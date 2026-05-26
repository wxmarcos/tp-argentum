#include "game.h"

#include "razas/humano.h"
#include "razas/elfo.h"
#include "razas/enano.h"
#include "razas/gnomo.h"
#include "clases/guerrero.h"
#include "clases/mago.h"
#include "clases/clerigo.h"
#include "clases/paladin.h"

Game::Game(Config& config, int anchoMapa, int altoMapa)
    : config(config), mapa(anchoMapa, altoMapa) {
    inicializarRazas();
    inicializarClases();
}

void Game::inicializarRazas() {
    razas["humano"] = std::make_unique<Humano>(config);
    razas["elfo"] = std::make_unique<Elfo>(config);
    razas["enano"] = std::make_unique<Enano>(config);
    razas["gnomo"] = std::make_unique<Gnomo>(config);
}

void Game::inicializarClases() {
    clases["guerrero"] = std::make_unique<Guerrero>(config);
    clases["mago"] = std::make_unique<Mago>(config);
    clases["clerigo"] = std::make_unique<Clerigo>(config);
    clases["paladin"] = std::make_unique<Paladin>(config);
}


bool Game::agregarJugador(const std::string& nombre, int posX, int posY,
                           const std::string& razaNombre, const std::string& claseNombre) {
    if (jugadores.count(nombre)) return false;

    auto itRaza = razas.find(razaNombre);
    auto itClase = clases.find(claseNombre);
    if (itRaza == razas.end() || itClase == clases.end()) return false;

    auto jugador = std::make_unique<Jugador>(
        nombre, posX, posY,
        itRaza->second.get(),
        itClase->second.get()
    );

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
    jugador->interrumpirMeditacion();
    return mapa.moverPersonaje(jugador, dir);
}

void Game::tick(float dt) {
    for (auto& [nombre, jugador] : jugadores) {
        jugador->recuperacionPasiva(dt);
    }

    // TODO: tick de criaturas (movimiento, ataque)
}

const Mapa& Game::getMapa() const { return mapa; }

bool Game::tirarItem(const std::string& nombre, int indice, int cantidad) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador || !jugador->estaVivo()) return false;

    auto slot = jugador->soltarItem(indice, cantidad);
    if(!slot) return false;

    mapa.tirarItem(jugador->getPosX(), jugador->getPosY(), std::move(*slot));
    return true;
}

bool Game::tomarItem(const std::string& nombre, int indice) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador || !jugador->estaVivo()) return false;
    if (jugador->getInventario().estaLleno()) return false;

    auto slot = mapa.tomarItemEnPosicion(jugador->getPosX(), jugador->getPosY(), indice);
    if (!slot) return false;

    jugador->agarrarItem(std::move(slot->item), slot->cantidad);
    return true;
}