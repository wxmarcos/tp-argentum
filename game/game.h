#pragma once
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/command/command.h"
#include "common/snapshot/snapshot.h"
#include "game/banco/cuentaBanco.h"
#include "game/characters/jugador.h"
#include "game/clases/charClase.h"
#include "game/config.h"
#include "game/criaturas/criatura.h"
#include "game/mundo.h"
#include "game/razas/raza.h"
#include "server/persistence/persistence_task.h"
#include "server/persistence/persistence_task_factory.h"

struct ResultadoAtaque {
    bool exito;
    int danioAplicado;
    bool fueEsquivado;
    bool fueCritico;
    bool objetivoMurio;
};

class Game {
private:
    Config& config;
    Mundo mundo;

    std::map<std::string, std::unique_ptr<Raza>> razas;
    std::map<std::string, std::unique_ptr<charClase>> clases;
    std::map<std::string, std::unique_ptr<Jugador>> jugadores;
    std::map<std::string, std::unique_ptr<Criatura>> criaturas;
    int nextCriaturaId;

    // Spawn
    float tiempoDesdeUltimoSpawn;
    struct InfoSpawnMapa {
        int poblacionMax;
        std::vector<std::string> criaturasPosibles;
    };
    std::map<int, InfoSpawnMapa> infoSpawn;

    std::unordered_map<uint16_t, std::string> player_id_to_nick;

    void cargarMundo();
    void inicializarRazas();
    void inicializarClases();
    void cargarJugadoresPersistidos();
    std::string to_lower(const std::string& str) const;
    bool restaurarJugadorPersistido(const PersistenceTask& player);

    bool puedeAtacarJugador(Jugador* atacante, Jugador* objetivo);
    // Helpers
    std::string getNombreJugadorPorComando(const Command& cmd) const;
    void agregarReplayDeJugadores(std::vector<Snapshot>& snapshots,
                                  const std::string& nickQueEntra) const;
    bool handle_meditation_interruption(Jugador* jugador,
                                        std::vector<Snapshot>& snapshots,
                                        const std::string& nombre);
    std::unique_ptr<Item> crear_item_por_nombre(const std::string& nombre);
    // Combate contra criaturas (logica separada de PvP)
    ResultadoAtaque atacarCriatura(Jugador* atacante, Criatura* objetivo);
    void procesarDropCriatura(Jugador* atacante, Criatura* criatura);

    // IA de criaturas
    void tickCriaturas(float dt, std::vector<Snapshot>& snapshots);
    int criaturaAtacaJugador(Criatura* criatura, Jugador* jugador);
    void spawnCriaturas();

    // NPC y comportamiento
    struct InfoNPC {
        int mapaId, x, y;
    };

    std::vector<InfoNPC> sacerdotes;
    std::vector<InfoNPC> comerciantes;
    std::vector<InfoNPC> banqueros;

    std::map<std::string, CuentaBanco> cuentasBancarias;

    void cargarNPCs();
    bool encontrarSacerdoteMasCercano(const Jugador* fantasma, InfoNPC& destino,
                                      float& distancia) const;
    bool hayNPCCercano(const Jugador* jugador,
                       const std::vector<InfoNPC>& npcs) const;
    void tickResucitando(float dt, std::vector<Snapshot>& snapshots);

public:
    explicit Game(Config& config);

    std::vector<PersistenceTask> build_persistence_tasks_for_command(
        const Command& cmd) const;
    std::vector<Snapshot> process(const Command& cmd);

    // Jugadores
    bool agregarJugador(const std::string& nombre, int mapaId, int posX,
                        int posY, const std::string& razaNombre,
                        const std::string& claseNombre);
    void removerJugador(const std::string& nombre);
    Jugador* getJugador(const std::string& nombre);
    const Jugador* getJugador(const std::string& nombre) const;
    bool moverJugador(const std::string& nombre, Direccion dir);

    // Criaturas
    std::string agregarCriatura(const std::string& tipo, int mapaId, int posX,
                                int posY);
    void removerCriatura(const std::string& id);
    Criatura* getCriatura(const std::string& id);

    // Combate
    ResultadoAtaque atacar(const std::string& nombreAtacante,
                           const std::string& nombreObjetivo);

    std::vector<Snapshot> tick(float dt);
    const Mundo& getMundo() const;

    bool tirarItem(const std::string& nombre, int indice, int cantidad = -1);
    std::optional<int> tomarItem(const std::string& nombre, int indice);
};
