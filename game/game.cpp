#include "game.h"

Game::Game(int anchoMapa, int altoMapa)
    : mapa(anchoMapa, altoMapa) {}

bool Game::agregarJugador(std::unique_ptr<Jugador> jugador) {
    const std::string& nombre = jugador->getNombre();
    if (jugadores.count(nombre)) return false;

    mapa.agregarPersonaje(jugador.get());
    jugadores[nombre] = std::move(jugador);
    return true;
}

void Game::removerJugador(const std::string& nombre) {
    auto it = jugadores.find(nombre);
    if (it == jugadores.end()) return;

    mapa.removerPersonaje(it->second.get());
    jugadores.erase(it);
}

Jugador* Game::getJugador(const std::string& nombre) {
    auto it = jugadores.find(nombre);
    if (it == jugadores.end()) return nullptr;
    return it->second.get();
}

bool Game::moverJugador(const std::string& nombre, Direccion dir) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador) return false;
    return mapa.moverPersonaje(jugador, dir);
}

void Game::tick() {
    for (auto& [nombre, jugador] : jugadores) {
        jugador->recuperacionPasiva();
    }

    // TODO: tick de criaturas (movimiento, ataque)
}

const Mapa& Game::getMapa() const { return mapa; }