#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "config.h"
#include "mundo.h"
#include "characters/jugador.h"
#include "razas/raza.h"
#include "clases/charClase.h"
#include "common/command.h"
#include "common/snapshot.h"

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
    
    std::map<std::string, std::unique_ptr<Raza>> razas;
    std::map<std::string, std::unique_ptr<charClase>> clases;

    Mundo mundo;
    std::map<std::string, std::unique_ptr<Jugador>> jugadores;
    std::unordered_map<uint16_t, std::string> player_id_to_nick;

    void cargarMundo();
    void inicializarRazas();
    void inicializarClases();

    bool puedeAtacarJugador(Jugador* atacante, Jugador* objetivo);
    std::string getNombreJugadorPorComando(const Command& cmd) const;
    Snapshot build_entity_move_snapshot(const std::string& nombre) const;

    // TODO: criaturas
    // TODO: npcs

public:
    Game(Config& config);
  
    std::vector<Snapshot> process(const Command& cmd);

    bool agregarJugador(const std::string& nombre, int mapaId, int posX, int posY,
                        const std::string& razaNombre, const std::string& claseNombre);
    void removerJugador(const std::string& nombre);
    Jugador* getJugador(const std::string& nombre);
    const Jugador* getJugador(const std::string& nombre) const;

    bool moverJugador(const std::string& nombre, Direccion dir);
    ResultadoAtaque atacar(const std::string& nombreAtacante, const std::string& nombreObjetivo);

    void tick(float dt);

    const Mundo& getMundo() const;

    bool tirarItem(const std::string& nombre, int indice, int cantidad = -1);
    bool tomarItem(const std::string& nombre, int indice);
};