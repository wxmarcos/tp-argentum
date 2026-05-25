#pragma once
#include <map>
#include <memory>
#include <string>
#include "map.h"
#include "characters/jugador.h"

class Game {
private:
    Mapa mapa;
    std::map<std::string, std::unique_ptr<Jugador>> jugadores;
    // TODO: criaturas
    // TODO: npcs

public:
    Game(int anchoMapa, int altoMapa);

    bool agregarJugador(std::unique_ptr<Jugador> jugador);
    void removerJugador(const std::string& nombre);
    Jugador* getJugador(const std::string& nombre);

    bool moverJugador(const std::string& nombre, Direccion dir);

    void tick();

    const Mapa& getMapa() const;
};