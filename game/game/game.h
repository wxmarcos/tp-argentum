#pragma once
#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/command/command.h"
#include "common/snapshot/snapshot.h"
#include "game/banco/cuentaBanco.h"
#include "game/characters/jugador.h"
#include "game/clan.h"
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

struct ResultadoTomarItem {
    bool exito = false;
    int slotInventario = -2;  // -1 oro, >=0 slot, -2 error
    std::string itemNombre;
    uint16_t cantidad = 0;
};

class Game {
private:
    // ---- Estado general ----
    Config& config;
    Mundo mundo;

    std::map<std::string, std::unique_ptr<Raza>> razas;
    std::map<std::string, std::unique_ptr<charClase>> clases;
    std::map<std::string, std::unique_ptr<Jugador>> jugadores;
    std::map<std::string, std::unique_ptr<Criatura>> criaturas;
    std::map<std::string, Clan> clanes;
    int nextCriaturaId;

    std::unordered_map<uint16_t, std::string> player_id_to_nick;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point>
        last_move_by_player;

    // ---- NPCs ----
    struct InfoNPC {
        int mapaId, x, y;
    };

    std::vector<InfoNPC> sacerdotes;
    std::vector<InfoNPC> comerciantes;
    std::vector<InfoNPC> banqueros;

    std::map<std::string, CuentaBanco> cuentasBancarias;

    // ---- Spawn de criaturas ----
    float tiempoDesdeUltimoSpawn;
    struct InfoSpawnMapa {
        int poblacionMax;
        std::vector<std::string> criaturasPosibles;
    };
    std::map<int, InfoSpawnMapa> infoSpawn;

    // ---- Inicialización (game.cpp) ----
    void cargarMundo();
    void inicializarRazas();
    void inicializarClases();
    void cargarNPCs();
    std::string to_lower(const std::string& str) const;
    bool restaurarJugadorPersistido(const PersistenceTask& player);
    bool puedeMoverAhora(const std::string& nombre);

    // ---- Helpers de replay/protocolo (game.cpp) ----
    std::string getNombreJugadorPorComando(const Command& cmd) const;
    void agregarReplayDeJugadores(std::vector<Snapshot>& snapshots,
                                  const std::string& nickQueEntra,
                                  int mapaId) const;
    void agregarReplayCriaturas(std::vector<Snapshot>& snapshots,
                                int mapaId) const;
    void agregarReplayNpcs(std::vector<Snapshot>& snapshots, int mapaId) const;
    void agregarReplayItems(std::vector<Snapshot>& snapshots,
                            int mapaId) const;
    bool handle_meditation_interruption(Jugador* jugador,
                                        std::vector<Snapshot>& snapshots,
                                        const std::string& nombre);
    std::unique_ptr<Item> crear_item_por_nombre(const std::string& nombre);

    // ---- Combate (game_combat.cpp) ----
    bool puedeAtacarJugador(Jugador* atacante, Jugador* objetivo);
    ResultadoAtaque atacarCriatura(Jugador* atacante, Criatura* objetivo);
    void procesarDropCriatura(const std::string& criaturaId, Jugador* atacante,
                              Criatura* criatura,
                              std::vector<Snapshot>& snapshots);
    int criaturaAtacaJugador(Criatura* criatura, Jugador* jugador);
    int contarCompañerosClanEnMapa(const Jugador* jugador) const;

    // ---- IA / mundo (game_world.cpp) ----
    void spawnCriaturas(std::vector<Snapshot>& snapshots);
    void tickCriaturas(float dt, std::vector<Snapshot>& snapshots);
    bool encontrarSacerdoteMasCercano(const Jugador* fantasma, InfoNPC& destino,
                                      float& distancia) const;
    void tickResucitando(float dt, std::vector<Snapshot>& snapshots);

    // ---- Helpers de comandos (game_commands.cpp) ----
    bool hayNPCCercano(const Jugador* jugador,
                       const std::vector<InfoNPC>& npcs) const;
    bool tirarItem(const std::string& nombre, int indice, int cantidad = -1);
    ResultadoTomarItem tomarItem(const std::string& nombre, int indice);

    // ---- Handlers de items (game_commands_items.cpp) ----
    void handleMover(const std::string& nombre, const Command& cmd,
                     std::vector<Snapshot>& snapshots);
    void handlePickItem(const std::string& nombre, const Command& cmd,
                        std::vector<Snapshot>& snapshots);
    void handleDropItem(const std::string& nombre, const Command& cmd,
                        std::vector<Snapshot>& snapshots);
    void handleEquipItem(const std::string& nombre, const Command& cmd,
                         std::vector<Snapshot>& snapshots);

    // ---- Handlers de comercio (game_commands_commerce.cpp) ----
    void handleBuyItem(const std::string& nombre, const Command& cmd,
                       std::vector<Snapshot>& snapshots);
    void handleSellItem(const std::string& nombre, const Command& cmd,
                        std::vector<Snapshot>& snapshots);
    void handleDepositItem(const std::string& nombre, const Command& cmd,
                           std::vector<Snapshot>& snapshots);
    void handleWithdrawItem(const std::string& nombre, const Command& cmd,
                            std::vector<Snapshot>& snapshots);
    void handleDepositGold(const std::string& nombre, const Command& cmd,
                           std::vector<Snapshot>& snapshots);
    void handleWithdrawGold(const std::string& nombre, const Command& cmd,
                            std::vector<Snapshot>& snapshots);
    void handleListItems(const std::string& nombre,
                         std::vector<Snapshot>& snapshots);

    // ---- Handlers de clanes (game_commands_clan.cpp) ----
    void handleClanCreate(const std::string& nombre, const Command& cmd,
                          std::vector<Snapshot>& snapshots);
    void handleClanJoin(const std::string& nombre, const Command& cmd,
                        std::vector<Snapshot>& snapshots);
    void handleClanReview(const std::string& nombre,
                          std::vector<Snapshot>& snapshots);
    void handleClanAccept(const std::string& nombre, const Command& cmd,
                          std::vector<Snapshot>& snapshots);
    void handleClanReject(const std::string& nombre, const Command& cmd,
                          std::vector<Snapshot>& snapshots);
    void handleClanBanKick(const std::string& nombre, const Command& cmd,
                           std::vector<Snapshot>& snapshots);
    void handleClanLeave(const std::string& nombre,
                         std::vector<Snapshot>& snapshots);

public:
    explicit Game(Config& config);

    // Tick del juego (game.cpp)
    std::vector<Snapshot> tick(float dt);

    // Procesar un comando de cliente (game_commands.cpp)
    std::vector<Snapshot> process(const Command& cmd);

    // Persistencia (game.cpp)
    std::vector<PersistenceTask> build_persistence_tasks_for_command(
        const Command& cmd) const;
    std::vector<PersistenceTask> build_all_players_tasks() const;

    // Jugadores (game.cpp)
    bool agregarJugador(const std::string& nombre, int mapaId, int posX,
                        int posY, const std::string& razaNombre,
                        const std::string& claseNombre);
    void removerJugador(const std::string& nombre);
    Jugador* getJugador(const std::string& nombre);
    const Jugador* getJugador(const std::string& nombre) const;
    bool moverJugador(const std::string& nombre, Direccion dir);

    // Criaturas (game.cpp)
    std::string agregarCriatura(const std::string& tipo, int mapaId, int posX,
                                int posY);
    void removerCriatura(const std::string& id);
    Criatura* getCriatura(const std::string& id);

    // Combate PvP/PvE (game_combat.cpp)
    ResultadoAtaque atacar(const std::string& nombreAtacante,
                           const std::string& nombreObjetivo);

    // Mundo (game.cpp)
    const Mundo& getMundo() const;
};
