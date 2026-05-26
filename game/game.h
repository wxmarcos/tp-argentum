#pragma once

#include <map>
#include <memory>
#include <string>

#include "config.h"
#include "map.h"
#include "characters/jugador.h"
#include "razas/raza.h"
#include "clases/charClase.h"

class Game {
private:
    Config& config;

    std::map<std::string, std::unique_ptr<Raza>> razas;
    std::map<std::string, std::unique_ptr<CharClase>> clases;

    Mapa mapa;
    std::map<std::string, std::unique_ptr<Jugador>> jugadores;

    void inicializarRazas();
    void inicializarClases();

    // TODO: criaturas
    // TODO: npcs

public:
    Game(Config& config, int anchoMapa, int altoMapa);

    bool agregarJugador(const std::string& nombre, int posX, int posY,
                        const std::string& razaNombre, const std::string& claseNombre);
    void removerJugador(const std::string& nombre);
    Jugador* getJugador(const std::string& nombre);

    bool moverJugador(const std::string& nombre, Direccion dir);

    void tick(float dt);

    const Mapa& getMapa() const;

    bool tirarItem(const std::string& nombre, int indice, int cantidad = -1);
    bool tomarItem(const std::string& nombre, int indice);
};