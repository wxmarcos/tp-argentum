#include "game/game.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include "common/protocol_defs.h"
#include "game/clases/clerigo.h"
#include "game/clases/guerrero.h"
#include "game/clases/mago.h"
#include "game/clases/paladin.h"
#include "game/criaturas/arana.h"
#include "game/criaturas/esqueleto.h"
#include "game/criaturas/goblin.h"
#include "game/criaturas/golem.h"
#include "game/criaturas/orco.h"
#include "game/criaturas/zombie.h"
#include "game/formulas.h"
#include "game/items/arma.h"
#include "game/items/armadura.h"
#include "game/items/casco.h"
#include "game/items/escudo.h"
#include "game/items/inventario.h"
#include "game/items/itemFactory.h"
#include "game/items/item_defs.h"
#include "game/items/oro.h"
#include "game/razas/elfo.h"
#include "game/razas/enano.h"
#include "game/razas/gnomo.h"
#include "game/razas/humano.h"
#include "game/snapshot_factory.h"
#include "game/tmx_loader.h"
#include "server/persistence/persistence_loader.h"

// ----------------- Constructor -----------------
Game::Game(Config& config):
    config(config), nextCriaturaId(0),
    tiempoDesdeUltimoSpawn(config.getSpawnIntervalo()) {
    inicializarRazas();
    inicializarClases();
    cargarMundo();

    for (const auto& cm : config.getMapas()) {
        infoSpawn[cm.id] = {cm.poblacionMax, cm.criaturasPosibles};
    }
    cargarNPCs();
}

// ----------------- Inicializacion -----------------

void Game::cargarMundo() {
    for (const auto& cm : config.getMapas()) {
        auto mapa = std::make_unique<Mapa>(cm.ancho, cm.alto, cm.esZonaSegura);

        if (!cm.archivoTmx.empty()) {
            try {
                TmxLoader::cargarColisiones(cm.archivoTmx, *mapa);
                std::cout << "[Game] colisiones cargadas desde "
                          << cm.archivoTmx << "\n";
            } catch (const std::exception& e) {
                std::cout << "[Game] WARNING: " << e.what() << "\n";
            }
        }

        for (const auto& p : cm.portales) {
            mapa->registrarPortal(p.x, p.y, p.mapaDestino, p.destinoX,
                                  p.destinoY);
        }

        infoMapasVecinos vecinos{cm.vecinoNorte, cm.vecinoSur, cm.vecinoEste,
                                 cm.vecinoOeste};

        mundo.agregarMapa(cm.id, std::move(mapa), vecinos);
    }
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

// ----------------- Persistencia -----------------

std::string Game::to_lower(const std::string& str) const {
    std::string text = str;

    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });

    return text;
}
bool Game::puedeMoverAhora(const std::string& nombre) {
    static constexpr int MOVE_COOLDOWN_MS = 150;

    const auto now = std::chrono::steady_clock::now();

    auto it = last_move_by_player.find(nombre);

    if (it != last_move_by_player.end()) {
        const auto elapsed =
            std::chrono::duration_cast<std::chrono::milliseconds>(now -
                                                                  it->second);

        if (elapsed.count() < MOVE_COOLDOWN_MS) {
            return false;
        }
    }

    last_move_by_player[nombre] = now;
    return true;
}

bool Game::restaurarJugadorPersistido(const PersistenceTask& p) {
    bool ok = agregarJugador(p.nick, p.mapa_id, p.x, p.y, to_lower(p.raza),
                             to_lower(p.clase));

    if (!ok) {
        return false;
    }

    Jugador* jugador = getJugador(p.nick);
    if (!jugador) {
        return false;
    }

    jugador->setDireccion(static_cast<Direccion>(p.direction));

    jugador->restaurarEstado(p.nivel, p.vida, p.vida_max, p.mana, p.mana_max,
                             p.experiencia, p.oro, p.constitucion,
                             p.inteligencia, p.fuerza, p.agilidad);

    if (!p.clan_nombre.empty()) {
        jugador->setClanNombre(p.clan_nombre);
    }

    auto inventario = p.inventario;

    std::sort(
        inventario.begin(), inventario.end(),
        [](const auto& a, const auto& b) { return a.slot_id < b.slot_id; });

    for (const auto& item_persistido : inventario) {
        auto item = crear_item_por_nombre(item_persistido.item);

        if (!item) {
            std::cout << "[Game] item desconocido en persistencia: "
                      << item_persistido.item << "\n";
            continue;
        }

        std::optional<int> slot_agregado =
            jugador->agarrarItem(std::move(item), item_persistido.cantidad);

        if (!slot_agregado.has_value()) {
            continue;
        }
        int slot = item_persistido.slot_id;

        if (!item_persistido.equipado) {
            continue;
        }

        const auto& slots = jugador->getInventario().getSlots();

        if (slot < 0 || slot >= static_cast<int>(slots.size())) {
            continue;
        }

        if (!slots[slot].has_value()) {
            continue;
        }

        switch (slots[slot]->item->getTipo()) {
            case TipoItem::ARMA:
                jugador->equiparArma(slot);
                break;

            case TipoItem::BACULO:
                jugador->equiparBaculo(slot);
                break;

            case TipoItem::ARMADURA:
                jugador->equiparArmadura(slot);
                break;

            case TipoItem::CASCO:
                jugador->equiparCasco(slot);
                break;

            case TipoItem::ESCUDO:
                jugador->equiparEscudo(slot);
                break;

            default:
                break;
        }
        std::cout << "[Game] cargando item " << item_persistido.item
                  << " slot_persistido=" << item_persistido.slot_id
                  << " cantidad=" << item_persistido.cantidad
                  << " equipado=" << item_persistido.equipado << "\n";
    }

    return true;
}

void Game::cargarNPCs() {
    for (const auto& cm : config.getMapas()) {
        for (const auto& pos : cm.sacerdotes) {
            sacerdotes.push_back({cm.id, pos.x, pos.y});
        }
        for (const auto& pos : cm.comerciantes) {
            comerciantes.push_back({cm.id, pos.x, pos.y});
        }
        for (const auto& pos : cm.banqueros) {
            banqueros.push_back({cm.id, pos.x, pos.y});
        }
    }
}

// ----------------- Helpers internos -----------------

std::string Game::getNombreJugadorPorComando(const Command& cmd) const {
    auto it = player_id_to_nick.find(cmd.get_player_id());
    return (it != player_id_to_nick.end()) ? it->second : "";
}

void Game::agregarReplayDeJugadores(std::vector<Snapshot>& snapshots,
                                    const std::string& nickQueEntra,
                                    int mapaId) const {
    for (const auto& [nombre, otro] : jugadores) {
        if (nombre == nickQueEntra) {
            continue;
        }

        if (otro->getMapaId() != mapaId) {
            continue;
        }

        snapshots.push_back(Snapshot::entity_created(
            nombre, mapaId, static_cast<uint16_t>(otro->getPosX()),
            static_cast<uint16_t>(otro->getPosY()),
            static_cast<uint8_t>(otro->getDireccion())));
    }
}

void Game::agregarReplayNpcs(std::vector<Snapshot>& snapshots,
                             int mapaId) const {
    int id = 0;

    for (const auto& npc : sacerdotes) {
        if (npc.mapaId != mapaId) continue;

        snapshots.push_back(Snapshot::entity_created(
            "npc_sacerdote_" + std::to_string(id++), mapaId,
            static_cast<uint16_t>(npc.x), static_cast<uint16_t>(npc.y), 2));
    }

    for (const auto& npc : comerciantes) {
        if (npc.mapaId != mapaId) continue;

        snapshots.push_back(Snapshot::entity_created(
            "npc_comerciante_" + std::to_string(id++), mapaId,
            static_cast<uint16_t>(npc.x), static_cast<uint16_t>(npc.y), 2));
    }

    for (const auto& npc : banqueros) {
        if (npc.mapaId != mapaId) continue;

        snapshots.push_back(Snapshot::entity_created(
            "npc_banquero_" + std::to_string(id++), mapaId,
            static_cast<uint16_t>(npc.x), static_cast<uint16_t>(npc.y), 2));
    }
}

void Game::agregarReplayCriaturas(std::vector<Snapshot>& snapshots,
                                  int mapaId) const {
    for (const auto& [id, criatura] : criaturas) {
        if (criatura->getMapaId() != mapaId) continue;

        snapshots.push_back(Snapshot::entity_created(
            id, mapaId, static_cast<uint16_t>(criatura->getPosX()),
            static_cast<uint16_t>(criatura->getPosY()),
            static_cast<uint8_t>(criatura->getDireccion())));
    }
}

void Game::agregarReplayItems(std::vector<Snapshot>& snapshots,
                              int mapaId) const {
    const Mapa* mapa = mundo.getMapa(mapaId);
    if (!mapa) return;

    for (const auto& [pos, slots] : mapa->getItemsEnPiso()) {
        for (const auto& slot : slots) {
            if (!slot.item) continue;
            snapshots.push_back(Snapshot::item_event(
                static_cast<uint8_t>(protocol::ItemEventAction::DROP),
                "",  // sin entidad emisora
                slot.item->getNombre(),
                static_cast<uint16_t>(mapaId),
                static_cast<uint16_t>(pos.first),
                static_cast<uint16_t>(pos.second),
                static_cast<uint16_t>(slot.cantidad)));
        }
    }
}

bool Game::handle_meditation_interruption(Jugador* jugador,
                                          std::vector<Snapshot>& snapshots,
                                          const std::string& nombre) {
    if (!jugador || !jugador->estaMeditando()) return false;

    jugador->interrumpirMeditacion();
    snapshots.push_back(Snapshot::meditation_status(nombre, false));
    snapshots.push_back(SnapshotFactory::player_stats_from_player(*jugador));
    return true;
}

std::unique_ptr<Item> Game::crear_item_por_nombre(const std::string& nombre) {
    if (nombre == item_defs::ESPADA) return ItemFactory::crearEspada();
    if (nombre == item_defs::HACHA) return ItemFactory::crearHacha();
    if (nombre == item_defs::MARTILLO) return ItemFactory::crearMartillo();

    if (nombre == item_defs::VARA_DE_FRESNO)
        return ItemFactory::crearVaraDeFresno();
    if (nombre == item_defs::FLAUTA_ELFICA)
        return ItemFactory::crearFlautaElfica();
    if (nombre == item_defs::BACULO_NUDOSO)
        return ItemFactory::crearBaculoNudoso();
    if (nombre == item_defs::BACULO_ENGARZADO)
        return ItemFactory::crearBaculoEngarzado();

    if (nombre == item_defs::ARCO_SIMPLE) return ItemFactory::crearArcoSimple();
    if (nombre == item_defs::ARCO_COMPUESTO)
        return ItemFactory::crearArcoCompuesto();

    if (nombre == item_defs::ARMADURA_DE_CUERO)
        return ItemFactory::crearArmaduraDeCuero();
    if (nombre == item_defs::ARMADURA_DE_PLACAS)
        return ItemFactory::crearArmaduraDePlacas();
    if (nombre == item_defs::TUNICA_AZUL) return ItemFactory::crearTunicaAzul();

    if (nombre == item_defs::CAPUCHA) return ItemFactory::crearCapucha();
    if (nombre == item_defs::CASCO_DE_HIERRO)
        return ItemFactory::crearCascoDeHierro();
    if (nombre == item_defs::SOMBRERO_MAGICO)
        return ItemFactory::crearSombreroMagico();

    if (nombre == item_defs::ESCUDO_DE_TORTUGA)
        return ItemFactory::crearEscudoDeTortuga();
    if (nombre == item_defs::ESCUDO_DE_HIERRO)
        return ItemFactory::crearEscudoDeHierro();

    if (nombre == item_defs::POCION_DE_VIDA)
        return ItemFactory::crearPocionDeVida();
    if (nombre == item_defs::POCION_DE_MANA)
        return ItemFactory::crearPocionDeMana();

    return nullptr;
}

// ----------------- Gestion de jugadores -----------------

bool Game::agregarJugador(const std::string& nombre, int mapaId, int posX,
                          int posY, const std::string& razaNombre,
                          const std::string& claseNombre) {
    if (jugadores.count(nombre)) return false;

    auto itRaza = razas.find(razaNombre);
    auto itClase = clases.find(claseNombre);
    if (itRaza == razas.end() || itClase == clases.end()) return false;

    auto jugador = std::make_unique<Jugador>(
        nombre, posX, posY, itRaza->second.get(), itClase->second.get(),
        config.getInventarioCapacidadMax(), config.getFormulaExpCoeficiente(),
        config.getFormulaExpExponente(), config.getFormulaOroMaxCoeficiente(),
        config.getFormulaOroMaxExponente());
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
    return (it != jugadores.end()) ? it->second.get() : nullptr;
}

const Jugador* Game::getJugador(const std::string& nombre) const {
    auto it = jugadores.find(nombre);
    return (it != jugadores.end()) ? it->second.get() : nullptr;
}

bool Game::moverJugador(const std::string& nombre, Direccion dir) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador) return false;
    if (jugador->estaResucitando()) return false;
    jugador->interrumpirMeditacion();
    return mundo.moverPersonaje(jugador, dir);
}

// ----------------- Gestion de Criaturas -----------------

std::string Game::agregarCriatura(const std::string& tipo, int mapaId, int posX,
                                  int posY) {
    std::string id = tipo + "_" + std::to_string(nextCriaturaId++);

    std::unique_ptr<Criatura> criatura;
    if (tipo == "goblin")
        criatura = std::make_unique<Goblin>(config, posX, posY);
    else if (tipo == "esqueleto")
        criatura = std::make_unique<Esqueleto>(config, posX, posY);
    else if (tipo == "arana")
        criatura = std::make_unique<Arana>(config, posX, posY);
    else if (tipo == "golem")
        criatura = std::make_unique<Golem>(config, posX, posY);
    else if (tipo == "orco")
        criatura = std::make_unique<Orco>(config, posX, posY);
    else if (tipo == "zombie")
        criatura = std::make_unique<Zombie>(config, posX, posY);
    else
        return "";

    criatura->setMapaId(mapaId);
    mundo.agregarPersonaje(criatura.get());
    criaturas[id] = std::move(criatura);
    return id;
}

void Game::removerCriatura(const std::string& id) {
    auto it = criaturas.find(id);
    if (it == criaturas.end()) return;
    mundo.removerPersonaje(it->second.get());
    criaturas.erase(it);
}

Criatura* Game::getCriatura(const std::string& id) {
    auto it = criaturas.find(id);
    return (it != criaturas.end()) ? it->second.get() : nullptr;
}

// ----------------- Tick -----------------

std::vector<Snapshot> Game::tick(float dt) {
    std::vector<Snapshot> snapshots;

    for (auto& [nombre, jugador] : jugadores) {
        int oldVida = jugador->getVidaActual();
        int oldMana = jugador->getManaActual();

        jugador->recuperacionPasiva(dt);

        int nuevaVida = jugador->getVidaActual();
        int nuevaMana = jugador->getManaActual();

        if (oldVida != nuevaVida || oldMana != nuevaMana) {
            snapshots.push_back(
                SnapshotFactory::player_stats_from_player(*jugador));
        }
    }

    tickCriaturas(dt, snapshots);
    tickResucitando(dt, snapshots);

    tiempoDesdeUltimoSpawn += dt;
    if (tiempoDesdeUltimoSpawn >= config.getSpawnIntervalo()) {
        spawnCriaturas(snapshots);
        tiempoDesdeUltimoSpawn = 0.0f;
    }

    return snapshots;
}

// ----------------- Resto de metodos -----------------

const Mundo& Game::getMundo() const { return mundo; }

std::vector<PersistenceTask> Game::build_persistence_tasks_for_command(
    const Command& cmd) const {
    std::vector<PersistenceTask> tasks;
    const std::string actor = getNombreJugadorPorComando(cmd);
    for (const std::string& name :
         PersistenceTaskFactory::get_affected_players(cmd, actor)) {
        const Jugador* j = getJugador(name);
        if (j) tasks.push_back(PersistenceTaskFactory::from_player(*j));
    }
    return tasks;
}

std::vector<PersistenceTask> Game::build_all_players_tasks() const {
    std::vector<PersistenceTask> tasks;
    for (const auto& [nombre, jugador] : jugadores) {
        tasks.push_back(PersistenceTaskFactory::from_player(*jugador));
    }
    return tasks;
}
