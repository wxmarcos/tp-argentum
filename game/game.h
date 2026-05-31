#pragma once

#include <map>
#include <memory>
#include <string>

#include "config.h"
#include "mundo.h"
#include "characters/jugador.h"
#include "razas/raza.h"
#include "clases/charClase.h"

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
    std::map<std::string, std::unique_ptr<CharClase>> clases;

    Mundo mundo;
    std::map<std::string, std::unique_ptr<Jugador>> jugadores;

    void cargarMundo();

    void inicializarRazas();
    void inicializarClases();

    bool puedeAtacarJugador(Jugador* atacante, Jugador* objetivo);

    // TODO: criaturas
    // TODO: npcs

public:
    Game(Config& config);

    bool agregarJugador(const std::string& nombre, int mapaId, int posX, int posY,
                        const std::string& razaNombre, const std::string& claseNombre);
    void removerJugador(const std::string& nombre);
    Jugador* getJugador(const std::string& nombre);

    bool moverJugador(const std::string& nombre, Direccion dir);

    ResultadoAtaque atacar(const std::string& nombreAtacante, const std::string& nombreObjetivo);

    void tick(float dt);

    const Mundo& getMundo() const;

    bool tirarItem(const std::string& nombre, int indice, int cantidad = -1);
    bool tomarItem(const std::string& nombre, int indice);
};