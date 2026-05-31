#include "game.h"

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

bool Game::puedeAtacarJugador(Jugador* atacante, Jugador* objetivo) {
    if (atacante->getNivel() <= 12 || objetivo->getNivel() <= 12) return false;
    if (std::abs(atacante->getNivel() - objetivo->getNivel()) > 10) return false;
    return true;
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

ResultadoAtaque Game::atacar(const std::string& nombreAtacante, const std::string& nombreObjetivo) {
    ResultadoAtaque resultado{false, 0, false, false, false};

    Jugador* atacante = getJugador(nombreAtacante);
    Jugador* objetivo = getJugador(nombreObjetivo);
    if (!atacante || !objetivo || !atacante->estaVivo() || !objetivo->estaVivo()) return resultado;
    if (nombreAtacante == nombreObjetivo) return resultado;

    // Fair Play
    if (!puedeAtacarJugador(atacante, objetivo)) return resultado;

    resultado.exito = true;

    // Daño base
    int fuerza = atacante->getFuerza();
    const Arma* arma = atacante->getInventario().getArmaEquipada();
    int danio;
    if (arma) {
        danio = Formulas::calcularDanio(fuerza, arma->getDanioMin(), arma->getDanioMax());
    } else {
        danio = fuerza;
    }

    // Critico
    resultado.fueCritico = Formulas::calcularCritico();
    if (resultado.fueCritico) {
        danio *= 2;
    }

    // Esquive
    if (!resultado.fueCritico) {
        resultado.fueEsquivado = Formulas::calcularEsquive(objetivo->getAgilidad());
    }

    if (resultado.fueEsquivado) {
        resultado.danioAplicado = 0;
        return resultado;
    }

    // Defensa
    const Armadura* armadura = objetivo->getInventario().getArmaduraEquipada();
    const Casco* casco = objetivo->getInventario().getCascoEquipado();
    const Escudo* escudo = objetivo->getInventario().getEscudoEquipado();

    int defensa = Formulas::calcularDefensa(
        armadura ? armadura->getDefensaMin() : 0,
        armadura ? armadura->getDefensaMax() : 0,
        escudo ? escudo->getDefensaMin() : 0,
        escudo ? escudo->getDefensaMax() : 0,
        casco ? casco->getDefensaMin() : 0,
        casco ? casco->getDefensaMax() : 0
    );

    int danioFinal = std::max(0, danio - defensa);
    resultado.danioAplicado = danioFinal;

    objetivo->recibirDanio(danioFinal);

    // Experiencia
    int exp = Formulas::calcularExpAtaque(danioFinal, objetivo->getNivel(), atacante->getNivel());
    atacante->ganarExperiencia(exp);

    // Muerte
    resultado.objetivoMurio = !objetivo->estaVivo();
    if (resultado.objetivoMurio) {
        int expMatar = Formulas::calcularExpMatar(
            objetivo->getVidaMax(),
            objetivo->getNivel(),
            atacante->getNivel()
        );
        atacante->ganarExperiencia(expMatar);

        // Drop items al piso
        auto items = objetivo->soltarTodosLosItems();
        for (auto& item : items) {
            mapa.tirarItem(objetivo->getPosX(), objetivo->getPosY(), std::move(item));
        }
    }

    return resultado;
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