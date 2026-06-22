#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include "common/protocol_defs.h"
#include "game/formulas.h"
#include "game/game.h"
#include "game/items/arma.h"
#include "game/items/armadura.h"
#include "game/items/casco.h"
#include "game/items/escudo.h"
#include "game/items/inventario.h"
#include "game/items/itemFactory.h"
#include "game/items/item_defs.h"
#include "game/items/oro.h"
#include "game/snapshot_factory.h"

// ----------------- Combate PvP/PvE -----------------

bool Game::puedeAtacarJugador(Jugador* atacante, Jugador* objetivo) {
    if (atacante->getNivel() <= 12 || objetivo->getNivel() <= 12) return false;
    if (std::abs(atacante->getNivel() - objetivo->getNivel()) > 10)
        return false;

    // Zona segura
    Mapa* mapa = mundo.getMapa(atacante->getMapaId());
    if (mapa && mapa->esZonaSegura()) return false;

    // Mismo clan
    if (atacante->estaEnClan() &&
        atacante->getClanNombre() == objetivo->getClanNombre())
        return false;

    return true;
}

static std::unique_ptr<Item> crearArma() {
    using Fn = std::unique_ptr<Item> (*)();
    static const Fn items[] = {
        []() -> std::unique_ptr<Item> { return ItemFactory::crearEspada(); },
        []() -> std::unique_ptr<Item> { return ItemFactory::crearHacha(); },
        []() -> std::unique_ptr<Item> { return ItemFactory::crearMartillo(); },
    };
    int idx = rand() % (sizeof(items) / sizeof(items[0]));
    return items[idx]();
}

static std::unique_ptr<Item> crearEscudo() {
    using Fn = std::unique_ptr<Item> (*)();
    static const Fn items[] = {
        []() -> std::unique_ptr<Item> {
            return ItemFactory::crearEscudoDeTortuga();
        },
        []() -> std::unique_ptr<Item> {
            return ItemFactory::crearEscudoDeHierro();
        },
    };
    int idx = rand() % (sizeof(items) / sizeof(items[0]));
    return items[idx]();
}

static std::unique_ptr<Item> crearItemAleatorio() {
    using Fn = std::unique_ptr<Item> (*)();
    static const Fn items[] = {
        []() -> std::unique_ptr<Item> { return ItemFactory::crearEspada(); },
        []() -> std::unique_ptr<Item> { return ItemFactory::crearHacha(); },
        []() -> std::unique_ptr<Item> { return ItemFactory::crearMartillo(); },
        []() -> std::unique_ptr<Item> {
            return ItemFactory::crearArcoSimple();
        },
        []() -> std::unique_ptr<Item> {
            return ItemFactory::crearArcoCompuesto();
        },
        []() -> std::unique_ptr<Item> {
            return ItemFactory::crearVaraDeFresno();
        },
        []() -> std::unique_ptr<Item> {
            return ItemFactory::crearBaculoNudoso();
        },
        []() -> std::unique_ptr<Item> {
            return ItemFactory::crearBaculoEngarzado();
        },
        []() -> std::unique_ptr<Item> {
            return ItemFactory::crearArmaduraDeCuero();
        },
        []() -> std::unique_ptr<Item> {
            return ItemFactory::crearArmaduraDePlacas();
        },
        []() -> std::unique_ptr<Item> {
            return ItemFactory::crearTunicaAzul();
        },
        []() -> std::unique_ptr<Item> { return ItemFactory::crearCapucha(); },
        []() -> std::unique_ptr<Item> {
            return ItemFactory::crearCascoDeHierro();
        },
        []() -> std::unique_ptr<Item> {
            return ItemFactory::crearSombreroMagico();
        },
        []() -> std::unique_ptr<Item> {
            return ItemFactory::crearEscudoDeTortuga();
        },
        []() -> std::unique_ptr<Item> {
            return ItemFactory::crearEscudoDeHierro();
        },
    };
    int idx = rand() % (sizeof(items) / sizeof(items[0]));
    return items[idx]();
}

void Game::procesarDropCriatura(const std::string& criaturaId,
                                Jugador* /*atacante*/, Criatura* criatura,
                                std::vector<OutgoingSnapshot>& snapshots) {
    double r = static_cast<double>(rand()) / RAND_MAX;

    if (r < 0.80) return;

    int mx = criatura->getMapaId();
    int px = criatura->getPosX();
    int py = criatura->getPosY();

    // 0.80–0.87: oro (8%)
    if (r < 0.88) {
        int cantidad = Formulas::calcularOroDropNPC(criatura->getVidaMax(),
            config.getFormulaOroDropNPCDivisor());
        std::string nombreItem = item_defs::ORO;

        mundo.tirarItem(mx, px, py,
                        SlotInventario(ItemFactory::crearOro(cantidad)));

        push_broadcast(
            snapshots,
            Snapshot::item_event(
                static_cast<uint8_t>(protocol::ItemEventAction::DROP),
                criaturaId, nombreItem, static_cast<uint16_t>(mx),
                static_cast<uint16_t>(px), static_cast<uint16_t>(py),
                static_cast<uint16_t>(cantidad)));
        return;
    }

    // 0.88: poción (1%)
    if (r < 0.89) {
        bool esVida = rand() % 2 == 0;
        auto pocion = esVida ? ItemFactory::crearPocionDeVida()
                             : ItemFactory::crearPocionDeMana();

        std::string nombreItem = pocion->getNombre();

        mundo.tirarItem(mx, px, py, SlotInventario(std::move(pocion)));

        push_broadcast(
            snapshots,
            Snapshot::item_event(
                static_cast<uint8_t>(protocol::ItemEventAction::DROP),
                criaturaId, nombreItem, static_cast<uint16_t>(mx),
                static_cast<uint16_t>(px), static_cast<uint16_t>(py), 1));

        return;
    }

    // 0.89: item aleatorio (1%)
    if (r < 0.90) {
        auto item = crearItemAleatorio();
        std::string nombreItem = item->getNombre();

        mundo.tirarItem(mx, px, py, SlotInventario(std::move(item)));

        push_broadcast(
            snapshots,
            Snapshot::item_event(
                static_cast<uint8_t>(protocol::ItemEventAction::DROP),
                criaturaId, nombreItem, static_cast<uint16_t>(mx),
                static_cast<uint16_t>(px), static_cast<uint16_t>(py), 1));

        return;
    }

    // 0.90–0.94: arma (5%)
    if (r < 0.95) {
        auto item = crearArma();
        std::string nombreItem = item->getNombre();

        mundo.tirarItem(mx, px, py, SlotInventario(std::move(item)));

        push_broadcast(
            snapshots,
            Snapshot::item_event(
                static_cast<uint8_t>(protocol::ItemEventAction::DROP),
                criaturaId, nombreItem, static_cast<uint16_t>(mx),
                static_cast<uint16_t>(px), static_cast<uint16_t>(py), 1));

        return;
    }

    // 0.95–1.00: escudo (5%)
    auto item = crearEscudo();
    std::string nombreItem = item->getNombre();

    mundo.tirarItem(mx, px, py, SlotInventario(std::move(item)));

    push_broadcast(
        snapshots,
        Snapshot::item_event(
            static_cast<uint8_t>(protocol::ItemEventAction::DROP), criaturaId,
            nombreItem, static_cast<uint16_t>(mx), static_cast<uint16_t>(px),
            static_cast<uint16_t>(py), 1));
}

ResultadoAtaque Game::atacarCriatura(Jugador* atacante, Criatura* objetivo) {
    ResultadoAtaque resultado{false, 0, false, false, false};

    if (!atacante->estaVivo() || !objetivo->estaVivo()) return resultado;

    int fuerza = atacante->getFuerza();
    const Arma* arma = atacante->getInventario().getArmaEquipada();
    const Baculo* baculo = atacante->getInventario().getBaculoEquipado();

    // Flauta elfica: Se cura a si mismo, ignora objetivo
    if (baculo && baculo->getTipoHechizo() == TipoHechizo::CURACION) {
        if (!atacante->gastarMana(baculo->getCostoMana())) return resultado;
        int curacion = Formulas::calcularDanio(fuerza, baculo->getEfectoMin(),
                                               baculo->getEfectoMax());
        atacante->curar(curacion);
        resultado.danioAplicado = curacion;
        return resultado;
    }

    bool esAtaqueDeRango = (arma && arma->esDeRango()) || (baculo != nullptr);
    if (!esAtaqueDeRango) {
        int dx = std::abs(atacante->getPosX() - objetivo->getPosX());
        int dy = std::abs(atacante->getPosY() - objetivo->getPosY());
        if (dx + dy != 1) {
            resultado.fueraDeRango = true;
            return resultado;
        }
    }

    if (!arma && !baculo) return resultado;

    if (baculo && !atacante->gastarMana(baculo->getCostoMana())) {
        return resultado;
    }

    resultado.exito = true;

    int danio = arma ? Formulas::calcularDanio(fuerza, arma->getDanioMin(),
                                               arma->getDanioMax())
                     : Formulas::calcularDanio(fuerza, baculo->getEfectoMin(),
                                               baculo->getEfectoMax());

    resultado.fueCritico =
        Formulas::calcularCritico(config.getFormulaCriticoPorcentaje());
    if (resultado.fueCritico) danio *= 2;

    // Bonus grupal de clan del atacante
    int compAtacante = contarCompanerosClanEnMapa(atacante);
    danio = static_cast<int>(danio * (1.0 + compAtacante * 0.05));

    int danioFinal = danio;
    resultado.danioAplicado = danioFinal;
    objetivo->recibirDanio(danioFinal);

    atacante->ganarExperiencia(Formulas::calcularExpAtaque(
        danioFinal, objetivo->getNivel(), atacante->getNivel(),
        config.getFormulaExpNivelOffset()));

    resultado.objetivoMurio = !objetivo->estaVivo();

    if (resultado.objetivoMurio) {
        atacante->ganarExperiencia(Formulas::calcularExpMatar(
            objetivo->getVidaMax(), objetivo->getNivel(), atacante->getNivel(),
            config.getFormulaExpNivelOffset(), config.getFormulaExpMatarFactor()));
    }

    return resultado;
}

ResultadoAtaque Game::atacar(const std::string& nombreAtacante,
                             const std::string& nombreObjetivo) {
    ResultadoAtaque resultado{false, 0, false, false, false};

    Jugador* atacante = getJugador(nombreAtacante);
    if (!atacante || !atacante->estaVivo()) return resultado;
    if (nombreAtacante == nombreObjetivo) return resultado;

    // -- Objetivo es criatura ------------------------
    Criatura* criatura = getCriatura(nombreObjetivo);
    if (criatura) {
        return atacarCriatura(atacante, criatura);
    }

    // -- Objetivo es jugador -------------------------
    Jugador* objetivo = getJugador(nombreObjetivo);
    if (!objetivo || !objetivo->estaVivo()) return resultado;

    // Fair Play
    if (!puedeAtacarJugador(atacante, objetivo)) return resultado;

    int fuerza = atacante->getFuerza();
    const Arma* arma = atacante->getInventario().getArmaEquipada();
    const Baculo* baculo = atacante->getInventario().getBaculoEquipado();

    // Flauta elfica - se cura a si mismo, ignora objetivo
    if (baculo && baculo->getTipoHechizo() == TipoHechizo::CURACION) {
        if (!atacante->gastarMana(baculo->getCostoMana())) return resultado;
        int curacion = Formulas::calcularDanio(fuerza, baculo->getEfectoMin(),
                                               baculo->getEfectoMax());
        atacante->curar(curacion);
        resultado.danioAplicado = curacion;
        return resultado;
    }

    bool esAtaqueDeRango = (arma && arma->esDeRango()) || (baculo != nullptr);
    if (!esAtaqueDeRango) {
        int dx = std::abs(atacante->getPosX() - objetivo->getPosX());
        int dy = std::abs(atacante->getPosY() - objetivo->getPosY());
        if (dx + dy != 1) {
            resultado.fueraDeRango = true;
            return resultado;
        }  // no adyacente
    }

    if (!arma && !baculo) return resultado;

    if (baculo && !atacante->gastarMana(baculo->getCostoMana())) {
        return resultado;
    }
    resultado.exito = true;

    int danio = arma ? Formulas::calcularDanio(fuerza, arma->getDanioMin(),
                                               arma->getDanioMax())
                     : Formulas::calcularDanio(fuerza, baculo->getEfectoMin(),
                                               baculo->getEfectoMax());

    resultado.fueCritico =
        Formulas::calcularCritico(config.getFormulaCriticoPorcentaje());
    if (resultado.fueCritico) danio *= 2;

    // Bonus grupal de clan del atacante
    int compAtacante = contarCompanerosClanEnMapa(atacante);
    danio = static_cast<int>(danio * (1.0 + compAtacante * 0.05));

    if (!resultado.fueCritico)
        resultado.fueEsquivado =
            Formulas::calcularEsquive(objetivo->getAgilidad(),
                                      config.getFormulaEsquiveUmbral());

    if (resultado.fueEsquivado) {
        resultado.danioAplicado = 0;
        return resultado;
    }

    const Armadura* armadura = objetivo->getInventario().getArmaduraEquipada();
    const Casco* casco = objetivo->getInventario().getCascoEquipado();
    const Escudo* escudo = objetivo->getInventario().getEscudoEquipado();

    int defensa = Formulas::calcularDefensa(
        armadura ? armadura->getDefensaMin() : 0,
        armadura ? armadura->getDefensaMax() : 0,
        escudo ? escudo->getDefensaMin() : 0,
        escudo ? escudo->getDefensaMax() : 0,
        casco ? casco->getDefensaMin() : 0, casco ? casco->getDefensaMax() : 0);

    // Bonus grupal de clan del defensor
    int compDefensor = contarCompanerosClanEnMapa(objetivo);
    defensa = static_cast<int>(defensa * (1.0 + compDefensor * 0.05));

    int danioFinal = std::max(0, danio - defensa);
    resultado.danioAplicado = danioFinal;
    objetivo->recibirDanio(danioFinal);

    atacante->ganarExperiencia(Formulas::calcularExpAtaque(
        danioFinal, objetivo->getNivel(), atacante->getNivel(),
        config.getFormulaExpNivelOffset()));

    resultado.objetivoMurio = !objetivo->estaVivo();
    if (resultado.objetivoMurio) {
        objetivo->perderExperiencia(Formulas::calcularExpPerdida(
            objetivo->getExperiencia(),
            config.getFormulaExpPenalidadPorcentaje()));
        atacante->ganarExperiencia(Formulas::calcularExpMatar(
            objetivo->getVidaMax(), objetivo->getNivel(), atacante->getNivel(),
            config.getFormulaExpNivelOffset(), config.getFormulaExpMatarFactor()));
    }

    return resultado;
}

// ----------------- Criatura ataca jugador -----------------

int Game::criaturaAtacaJugador(Criatura* atacante, Jugador* objetivo) {
    if (!atacante || !objetivo) return 0;
    if (!atacante->estaVivo() || !objetivo->estaVivo()) return 0;

    if (Formulas::calcularEsquive(objetivo->getAgilidad(),
                                  config.getFormulaEsquiveUmbral())) {
        std::cout << "[ESQUIVE] " << objetivo->getNombre()
                  << " agi=" << objetivo->getAgilidad() << "\n";
        return 0;
    }

    int danio =
        Formulas::calcularDanio(atacante->getFuerza(), atacante->getDanioMin(),
                                atacante->getDanioMax());

    const Armadura* armadura = objetivo->getInventario().getArmaduraEquipada();
    const Casco* casco = objetivo->getInventario().getCascoEquipado();
    const Escudo* escudo = objetivo->getInventario().getEscudoEquipado();

    int defensa = Formulas::calcularDefensa(
        armadura ? armadura->getDefensaMin() : 0,
        armadura ? armadura->getDefensaMax() : 0,
        escudo ? escudo->getDefensaMin() : 0,
        escudo ? escudo->getDefensaMax() : 0,
        casco ? casco->getDefensaMin() : 0, casco ? casco->getDefensaMax() : 0);

    // Bonus grupal de clan del defensor
    int comp = contarCompanerosClanEnMapa(objetivo);
    defensa = static_cast<int>(defensa * (1.0 + comp * 0.05));

    int danioFinal = std::max(0, danio - defensa);

    objetivo->recibirDanio(danioFinal);

    return danioFinal;
}

// ----------------- Bonus grupal de clan -----------------

int Game::contarCompanerosClanEnMapa(const Jugador* jugador) const {
    if (!jugador->estaEnClan()) return 0;
    int count = 0;
    for (const auto& [nick, j] : jugadores) {
        if (nick == jugador->getNombre()) continue;
        if (!j->estaVivo()) continue;
        if (j->getClanNombre() != jugador->getClanNombre()) continue;
        if (j->getMapaId() != jugador->getMapaId()) continue;
        ++count;
    }
    return count;
}
