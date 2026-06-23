#include <algorithm>
#include <cmath>
#include <random>

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

static std::mt19937& rng() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

// ----------------- Combate PvP/PvE -----------------

bool Game::puedeAtacarJugador(Jugador* atacante, Jugador* objetivo) {
    int nivelMin = config.getPvpNivelMinimo();
    if (atacante->getNivel() <= nivelMin || objetivo->getNivel() <= nivelMin)
        return false;
    if (std::abs(atacante->getNivel() - objetivo->getNivel()) >
        config.getPvpDiferenciaNivelMax())
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

static std::unique_ptr<Item> crearArma(const Config& config) {
    static const char* const nombres[] = {
        item_defs::ESPADA,
        item_defs::HACHA,
        item_defs::MARTILLO,
    };
    int idx = std::uniform_int_distribution<int>(
        0, static_cast<int>(sizeof(nombres) / sizeof(nombres[0])) - 1)(rng());
    return ItemFactory::crear(nombres[idx], config);
}

static std::unique_ptr<Item> crearEscudo(const Config& config) {
    static const char* const nombres[] = {
        item_defs::ESCUDO_DE_TORTUGA,
        item_defs::ESCUDO_DE_HIERRO,
    };
    int idx = std::uniform_int_distribution<int>(
        0, static_cast<int>(sizeof(nombres) / sizeof(nombres[0])) - 1)(rng());
    return ItemFactory::crear(nombres[idx], config);
}

static std::unique_ptr<Item> crearItemAleatorio(const Config& config) {
    static const char* const nombres[] = {
        item_defs::ESPADA,
        item_defs::HACHA,
        item_defs::MARTILLO,
        item_defs::ARCO_SIMPLE,
        item_defs::ARCO_COMPUESTO,
        item_defs::VARA_DE_FRESNO,
        item_defs::BACULO_NUDOSO,
        item_defs::BACULO_ENGARZADO,
        item_defs::ARMADURA_DE_CUERO,
        item_defs::ARMADURA_DE_PLACAS,
        item_defs::TUNICA_AZUL,
        item_defs::CAPUCHA,
        item_defs::CASCO_DE_HIERRO,
        item_defs::SOMBRERO_MAGICO,
        item_defs::ESCUDO_DE_TORTUGA,
        item_defs::ESCUDO_DE_HIERRO,
    };
    int idx = std::uniform_int_distribution<int>(
        0, static_cast<int>(sizeof(nombres) / sizeof(nombres[0])) - 1)(rng());
    return ItemFactory::crear(nombres[idx], config);
}

void Game::procesarDropCriatura(const std::string& criaturaId,
                                Jugador* /*atacante*/, Criatura* criatura,
                                std::vector<OutgoingSnapshot>& snapshots) {
    double r = std::uniform_real_distribution<double>(0.0, 1.0)(rng());

    if (r < config.getDropUmbralNada()) return;

    int mx = criatura->getMapaId();
    int px = criatura->getPosX();
    int py = criatura->getPosY();

    if (r < config.getDropUmbralOro()) {
        int cantidad = Formulas::calcularOroDropNPC(
            criatura->getVidaMax(), config.getFormulaOroDropNPCDivisor());
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

    if (r < config.getDropUmbralPocion()) {
        bool esVida = std::uniform_int_distribution<int>(0, 1)(rng()) == 0;
        auto pocion = ItemFactory::crear(
            esVida ? item_defs::POCION_DE_VIDA : item_defs::POCION_DE_MANA,
            config);

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

    if (r < config.getDropUmbralItem()) {
        auto item = crearItemAleatorio(config);
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

    if (r < config.getDropUmbralArma()) {
        auto item = crearArma(config);
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

    auto item = crearEscudo(config);
    std::string nombreItem = item->getNombre();

    mundo.tirarItem(mx, px, py, SlotInventario(std::move(item)));

    push_broadcast(
        snapshots,
        Snapshot::item_event(
            static_cast<uint8_t>(protocol::ItemEventAction::DROP), criaturaId,
            nombreItem, static_cast<uint16_t>(mx), static_cast<uint16_t>(px),
            static_cast<uint16_t>(py), 1));
}

DanioCalculado Game::prepararAtaque(Jugador* atacante, Character* objetivo) {
    DanioCalculado resultado;

    int fuerza = atacante->getFuerza();
    const Arma* arma = atacante->getInventario().getArmaEquipada();
    const Baculo* baculo = atacante->getInventario().getBaculoEquipado();

    // Flauta elfica: Se cura a si mismo, ignora objetivo
    if (baculo && baculo->getTipoHechizo() == TipoHechizo::CURACION) {
        if (!atacante->gastarMana(baculo->getCostoMana())) return resultado;
        int curacion = Formulas::calcularDanio(fuerza, baculo->getEfectoMin(),
                                               baculo->getEfectoMax());
        atacante->curar(curacion);
        resultado.exito = true;
        resultado.esCuracionPropia = true;
        resultado.valor = curacion;
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

    if (baculo && !atacante->gastarMana(baculo->getCostoMana()))
        return resultado;

    resultado.exito = true;

    int danio = arma ? Formulas::calcularDanio(fuerza, arma->getDanioMin(),
                                               arma->getDanioMax())
                     : Formulas::calcularDanio(fuerza, baculo->getEfectoMin(),
                                               baculo->getEfectoMax());

    resultado.fueCritico =
        Formulas::calcularCritico(config.getFormulaCriticoPorcentaje());
    if (resultado.fueCritico) danio *= 2;

    int comp = contarCompanerosClanEnMapa(atacante);
    danio = static_cast<int>(danio * (1.0 + comp * 0.05));

    resultado.valor = danio;
    return resultado;
}

ResultadoAtaque Game::atacarCriatura(Jugador* atacante, Criatura* objetivo) {
    ResultadoAtaque resultado{false, 0, false, false, false};

    if (!atacante->estaVivo() || !objetivo->estaVivo()) return resultado;

    DanioCalculado d = prepararAtaque(atacante, objetivo);
    if (d.esCuracionPropia) {
        resultado.danioAplicado = d.valor;
        return resultado;
    }
    if (!d.exito) {
        resultado.fueraDeRango = d.fueraDeRango;
        return resultado;
    }

    resultado.exito = true;
    resultado.fueCritico = d.fueCritico;
    resultado.danioAplicado = d.valor;
    objetivo->recibirDanio(d.valor);

    atacante->ganarExperiencia(Formulas::calcularExpAtaque(
        d.valor, objetivo->getNivel(), atacante->getNivel(),
        config.getFormulaExpNivelOffset()));

    resultado.objetivoMurio = !objetivo->estaVivo();

    if (resultado.objetivoMurio) {
        atacante->ganarExperiencia(Formulas::calcularExpMatar(
            objetivo->getVidaMax(), objetivo->getNivel(), atacante->getNivel(),
            config.getFormulaExpNivelOffset(),
            config.getFormulaExpMatarFactor()));
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

    DanioCalculado d = prepararAtaque(atacante, objetivo);
    if (d.esCuracionPropia) {
        resultado.danioAplicado = d.valor;
        return resultado;
    }
    if (!d.exito) {
        resultado.fueraDeRango = d.fueraDeRango;
        return resultado;
    }

    resultado.exito = true;
    resultado.fueCritico = d.fueCritico;

    if (!resultado.fueCritico)
        resultado.fueEsquivado = Formulas::calcularEsquive(
            objetivo->getAgilidad(), config.getFormulaEsquiveUmbral());

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

    int compDefensor = contarCompanerosClanEnMapa(objetivo);
    defensa = static_cast<int>(
        defensa * (1.0 + compDefensor * config.getPvpBonusClaPorComp()));

    int danioFinal = std::max(0, d.valor - defensa);
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
            config.getFormulaExpNivelOffset(),
            config.getFormulaExpMatarFactor()));
    }

    return resultado;
}

// ----------------- Criatura ataca jugador -----------------

int Game::criaturaAtacaJugador(Criatura* atacante, Jugador* objetivo) {
    if (!atacante || !objetivo) return 0;
    if (!atacante->estaVivo() || !objetivo->estaVivo()) return 0;

    if (Formulas::calcularEsquive(objetivo->getAgilidad(),
                                  config.getFormulaEsquiveUmbral())) {
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
    defensa = static_cast<int>(defensa *
                               (1.0 + comp * config.getPvpBonusClaPorComp()));

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
