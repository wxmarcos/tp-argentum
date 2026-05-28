#include "game.h"

#include "common/protocol_defs.h"

#include "razas/humano.h"
#include "razas/elfo.h"
#include "razas/enano.h"
#include "razas/gnomo.h"

#include "clases/guerrero.h"
#include "clases/mago.h"
#include "clases/clerigo.h"
#include "clases/paladin.h"

Game::Game(
    Config& config,
    int anchoMapa,
    int altoMapa)
    :
    config(config),
    mapa(anchoMapa, altoMapa) {

    inicializarRazas();
    inicializarClases();
}

void Game::inicializarRazas() {

    razas["humano"] =
        std::make_unique<Humano>(config);

    razas["elfo"] =
        std::make_unique<Elfo>(config);

    razas["enano"] =
        std::make_unique<Enano>(config);

    razas["gnomo"] =
        std::make_unique<Gnomo>(config);
}

void Game::inicializarClases() {

    clases["guerrero"] =
        std::make_unique<Guerrero>(config);

    clases["mago"] =
        std::make_unique<Mago>(config);

    clases["clerigo"] =
        std::make_unique<Clerigo>(config);

    clases["paladin"] =
        std::make_unique<Paladin>(config);
}

std::string Game::getNombreJugadorPorComando(
    const Command& cmd) const {

    // Temporal:
    // Más adelante:
    // player_id -> nick

    (void) cmd;

    if (jugadores.empty()) {
        return "";
    }

    return jugadores.begin()->first;
}

Snapshot Game::build_entity_move_snapshot(
    const std::string& nombre) const {

    const Jugador* jugador =
        getJugador(nombre);

    if (!jugador) {
        return Snapshot::entity_remove(nombre);
    }

    return Snapshot::entity_move(
        nombre,
        static_cast<uint16_t>(
            jugador->getPosX()),
        static_cast<uint16_t>(
            jugador->getPosY()),
        static_cast<uint8_t>(
            jugador->getDireccion()));
}

std::vector<Snapshot> Game::process(
    const Command& cmd) {

    std::vector<Snapshot> snapshots;

    if (cmd.is_disconnect()) {

        const std::string nombre =
            getNombreJugadorPorComando(cmd);

        if (!nombre.empty()) {

            removerJugador(nombre);

            snapshots.push_back(
                Snapshot::entity_remove(nombre));
        }

        return snapshots;
    }

    const std::string nombre =
        getNombreJugadorPorComando(cmd);

    if (nombre.empty()) {
        return snapshots;
    }

    switch (cmd.get_type()) {

        // TODO
        case CommandType::Move:

            moverJugador(
                nombre,
                static_cast<Direccion>(
                    cmd.get_direction()));

            snapshots.push_back(
                build_entity_move_snapshot(nombre));

            break;

        case CommandType::PickItem:

            tomarItem(nombre, 0);

            snapshots.push_back(
                build_entity_move_snapshot(nombre));

            break;

        case CommandType::DropItem:

            tirarItem(
                nombre,
                cmd.get_item_id());

            snapshots.push_back(
                build_entity_move_snapshot(nombre));

            break;

        default:

            snapshots.push_back(
                build_entity_move_snapshot(nombre));

            break;
    }

    return snapshots;
}

bool Game::agregarJugador(
    const std::string& nombre,
    int posX,
    int posY,
    const std::string& razaNombre,
    const std::string& claseNombre) {

    if (jugadores.count(nombre))
        return false;

    auto itRaza =
        razas.find(razaNombre);

    auto itClase =
        clases.find(claseNombre);

    if (itRaza == razas.end() ||
        itClase == clases.end())
        return false;

    auto jugador =
        std::make_unique<Jugador>(
            nombre,
            posX,
            posY,
            itRaza->second.get(),
            itClase->second.get());

    mapa.agregarPersonaje(
        jugador.get());

    jugadores[nombre] =
        std::move(jugador);

    return true;
}

void Game::removerJugador(
    const std::string& nombre) {

    auto it =
        jugadores.find(nombre);

    if (it == jugadores.end())
        return;

    mapa.removerPersonaje(
        it->second.get());

    jugadores.erase(it);
}

Jugador* Game::getJugador(
    const std::string& nombre) {

    auto it =
        jugadores.find(nombre);

    if (it == jugadores.end())
        return nullptr;

    return it->second.get();
}

const Jugador* Game::getJugador(
    const std::string& nombre) const {

    auto it =
        jugadores.find(nombre);

    if (it == jugadores.end())
        return nullptr;

    return it->second.get();
}

bool Game::moverJugador(
    const std::string& nombre,
    Direccion dir) {

    Jugador* jugador =
        getJugador(nombre);

    if (!jugador)
        return false;

    jugador->interrumpirMeditacion();

    return mapa.moverPersonaje(
        jugador,
        dir);
}

void Game::tick(float dt) {

    for (auto& [nombre, jugador] : jugadores) {

        jugador->recuperacionPasiva(dt);
    }

    // TODO:
    // tick de criaturas
}

const Mapa& Game::getMapa() const {

    return mapa;
}

bool Game::tirarItem(
    const std::string& nombre,
    int indice,
    int cantidad) {

    Jugador* jugador =
        getJugador(nombre);

    if (!jugador ||
        !jugador->estaVivo())
        return false;

    auto slot =
        jugador->soltarItem(
            indice,
            cantidad);

    if (!slot)
        return false;

    mapa.tirarItem(
        jugador->getPosX(),
        jugador->getPosY(),
        std::move(*slot));

    return true;
}

bool Game::tomarItem(
    const std::string& nombre,
    int indice) {

    Jugador* jugador =
        getJugador(nombre);

    if (!jugador ||
        !jugador->estaVivo())
        return false;

    if (jugador->getInventario().estaLleno())
        return false;

    auto slot =
        mapa.tomarItemEnPosicion(
            jugador->getPosX(),
            jugador->getPosY(),
            indice);

    if (!slot)
        return false;

    jugador->agarrarItem(
        std::move(slot->item),
        slot->cantidad);

    return true;
}