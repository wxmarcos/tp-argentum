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
#include "items/arma.h"
#include "items/armadura.h"
#include "items/casco.h"
#include "items/escudo.h"
#include "formulas.h"
#include <cstdlib>

Game::Game(Config& config) : config(config) {
    inicializarRazas();
    inicializarClases();
    cargarMundo();
}

void Game::cargarMundo() {
    auto mapasConfig = config.getMapas();
    for (const auto& cm : mapasConfig) {
        auto mapa = std::make_unique<Mapa>(cm.ancho, cm.alto);
        infoMapasVecinos vecinos{cm.vecinoNorte, cm.vecinoSur, cm.vecinoEste, cm.vecinoOeste};
        mundo.agregarMapa(cm.id, std::move(mapa), vecinos);
    }
}

void Game::inicializarRazas() {
    razas["humano"] = std::make_unique<Humano>(config);
    razas["elfo"]   = std::make_unique<Elfo>(config);
    razas["enano"]  = std::make_unique<Enano>(config);
    razas["gnomo"]  = std::make_unique<Gnomo>(config);
}

void Game::inicializarClases() {
    clases["guerrero"] = std::make_unique<Guerrero>(config);
    clases["mago"]     = std::make_unique<Mago>(config);
    clases["clerigo"]  = std::make_unique<Clerigo>(config);
    clases["paladin"]  = std::make_unique<Paladin>(config);
}

std::string Game::getNombreJugadorPorComando(const Command& cmd) const {
    auto it = player_id_to_nick.find(cmd.get_player_id());
    if (it == player_id_to_nick.end()) return "";
    return it->second;
}

Snapshot Game::build_entity_move_snapshot(const std::string& nombre) const {
    const Jugador* jugador = getJugador(nombre);
    if (!jugador) return Snapshot::entity_remove(nombre);
    return Snapshot::entity_move(
        nombre,
        static_cast<uint16_t>(jugador->getPosX()),
        static_cast<uint16_t>(jugador->getPosY()),
        static_cast<uint8_t>(jugador->getDireccion()));
}

// TODO: mandar snapshots que tengan sentido con cada accion
std::vector<Snapshot> Game::process(const Command& cmd) {
    std::vector<Snapshot> snapshots;

    if (cmd.get_type() == CommandType::CreateCharacter) {
        bool creado = agregarJugador(
            cmd.get_nick(), 1, 10, 10,
            cmd.get_raza(), cmd.get_clase());
        if (creado) {
            player_id_to_nick[cmd.get_player_id()] = cmd.get_nick();
            snapshots.push_back(build_entity_move_snapshot(cmd.get_nick()));
        }
        return snapshots;
    }

    if (cmd.is_disconnect()) {
        const std::string nombre = getNombreJugadorPorComando(cmd);
        if (!nombre.empty()) {
            removerJugador(nombre);
            player_id_to_nick.erase(cmd.get_player_id());
            snapshots.push_back(Snapshot::entity_remove(nombre));
        }
        return snapshots;
    }

    const std::string nombre = getNombreJugadorPorComando(cmd);
    if (nombre.empty()) return snapshots;

    switch (cmd.get_type()) {
        case CommandType::Move: {
            bool moved = moverJugador(nombre,
                static_cast<Direccion>(cmd.get_direction()));
            if (moved)
                snapshots.push_back(build_entity_move_snapshot(nombre));
            break;
        }
        case CommandType::PickItem:
            if (tomarItem(nombre, 0)) {
                // TODO: devolver INVENTORY_UPDATE
            }
            break;
        case CommandType::DropItem:
            if (tirarItem(nombre, cmd.get_item_id())) {
                // TODO: devolver INVENTORY_UPDATE
            }
            break;
        default:
            break;
    }
    return snapshots;
}

bool Game::puedeAtacarJugador(Jugador* atacante, Jugador* objetivo) {
    if (atacante->getNivel() <= 12 || objetivo->getNivel() <= 12) return false;
    if (std::abs(atacante->getNivel() - objetivo->getNivel()) > 10) return false;
    return true;
}

bool Game::agregarJugador(const std::string& nombre, int mapaId, int posX, int posY,
                           const std::string& razaNombre, const std::string& claseNombre) {
    if (jugadores.count(nombre)) return false;

    auto itRaza  = razas.find(razaNombre);
    auto itClase = clases.find(claseNombre);
    if (itRaza == razas.end() || itClase == clases.end()) return false;

    auto jugador = std::make_unique<Jugador>(
        nombre, posX, posY,
        itRaza->second.get(),
        itClase->second.get());
    jugador->setMapaId(mapaId);
    mundo.agregarPersonaje(jugador.get());
    jugadores[nombre] = std::move(jugador);
    return true;
}

void Game::removerJugador(const std::string& nombre) {
    auto it = jugadores.find(nombre);
    if (it == jugadores.end()) return;
    mundo.removerPersonaje(it->second.get());
    jugadores.erase(it);
}

Jugador* Game::getJugador(const std::string& nombre) {
    auto it = jugadores.find(nombre);
    if (it == jugadores.end()) return nullptr;
    return it->second.get();
}

const Jugador* Game::getJugador(const std::string& nombre) const {
    auto it = jugadores.find(nombre);
    if (it == jugadores.end()) return nullptr;
    return it->second.get();
}

bool Game::moverJugador(const std::string& nombre, Direccion dir) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador) return false;
    jugador->interrumpirMeditacion();
    return mundo.moverPersonaje(jugador, dir);
}

ResultadoAtaque Game::atacar(const std::string& nombreAtacante,
                              const std::string& nombreObjetivo) {
    ResultadoAtaque resultado{false, 0, false, false, false};

    Jugador* atacante = getJugador(nombreAtacante);
    Jugador* objetivo = getJugador(nombreObjetivo);
    if (!atacante || !objetivo || !atacante->estaVivo() || !objetivo->estaVivo())
        return resultado;
    if (nombreAtacante == nombreObjetivo) return resultado;

    if (!puedeAtacarJugador(atacante, objetivo)) return resultado;
    resultado.exito = true;

    int fuerza = atacante->getFuerza();
    const Arma* arma = atacante->getInventario().getArmaEquipada();
    int danio = arma
        ? Formulas::calcularDanio(fuerza, arma->getDanioMin(), arma->getDanioMax())
        : fuerza;

    resultado.fueCritico = Formulas::calcularCritico();
    if (resultado.fueCritico) danio *= 2;

    if (!resultado.fueCritico)
        resultado.fueEsquivado = Formulas::calcularEsquive(objetivo->getAgilidad());

    if (resultado.fueEsquivado) {
        resultado.danioAplicado = 0;
        return resultado;
    }

    const Armadura* armadura = objetivo->getInventario().getArmaduraEquipada();
    const Casco*    casco    = objetivo->getInventario().getCascoEquipado();
    const Escudo*   escudo   = objetivo->getInventario().getEscudoEquipado();

    int defensa = Formulas::calcularDefensa(
        armadura ? armadura->getDefensaMin() : 0, armadura ? armadura->getDefensaMax() : 0,
        escudo   ? escudo->getDefensaMin()   : 0, escudo   ? escudo->getDefensaMax()   : 0,
        casco    ? casco->getDefensaMin()    : 0, casco    ? casco->getDefensaMax()    : 0);

    int danioFinal = std::max(0, danio - defensa);
    resultado.danioAplicado = danioFinal;
    objetivo->recibirDanio(danioFinal);

    atacante->ganarExperiencia(
        Formulas::calcularExpAtaque(danioFinal, objetivo->getNivel(), atacante->getNivel()));

    resultado.objetivoMurio = !objetivo->estaVivo();
    if (resultado.objetivoMurio) {
        atacante->ganarExperiencia(
            Formulas::calcularExpMatar(
                objetivo->getVidaMax(), objetivo->getNivel(), atacante->getNivel()));
        auto items = objetivo->soltarTodosLosItems();
        for (auto& item : items)
            mundo.tirarItem(objetivo->getMapaId(),
                            objetivo->getPosX(), objetivo->getPosY(), std::move(item));
    }
    return resultado;
}

void Game::tick(float dt) {
    for (auto& [nombre, jugador] : jugadores)
        jugador->recuperacionPasiva(dt);
    // TODO: tick de criaturas
}

const Mundo& Game::getMundo() const { return mundo; }

bool Game::tirarItem(const std::string& nombre, int indice, int cantidad) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador || !jugador->estaVivo()) return false;
    auto slot = jugador->soltarItem(indice, cantidad);
    if (!slot) return false;
    mundo.tirarItem(jugador->getMapaId(),
                    jugador->getPosX(), jugador->getPosY(), std::move(*slot));
    return true;
}

bool Game::tomarItem(const std::string& nombre, int indice) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador || !jugador->estaVivo()) return false;
    if (jugador->getInventario().estaLleno()) return false;
    auto slot = mundo.tomarItemEnPosicion(
        jugador->getMapaId(), jugador->getPosX(), jugador->getPosY(), indice);
    if (!slot) return false;
    jugador->agarrarItem(std::move(slot->item), slot->cantidad);
    return true;
}