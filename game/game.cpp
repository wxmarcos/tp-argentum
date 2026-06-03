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
#include "criaturas/arana.h"
#include "criaturas/esqueleto.h"
#include "criaturas/goblin.h"
#include "criaturas/golem.h"
#include "criaturas/orco.h"
#include "criaturas/zombie.h"
#include "items/arma.h"
#include "items/armadura.h"
#include "items/casco.h"
#include "items/escudo.h"
#include "items/itemFactory.h"
#include "formulas.h"
#include "server/persistence/persistance_loader.h"

#include <cstdlib>
#include <cmath>

static Snapshot make_player_stats(Jugador* j);

// ----------------- Constructor -----------------
Game::Game(Config& config)
    : config(config), nextCriaturaId(0),
        tiempoDesdeUltimoSpawn(config.getSpawnIntervalo()) {
    inicializarRazas();
    inicializarClases();
    cargarMundo();
    cargarJugadoresPersistidos();

    for (const auto& cm : config.getMapas()) {
        infoSpawn[cm.id] = {cm.poblacionMax, cm.criaturasPosibles};
    }
    cargarSacerdotes();
}

// ----------------- Inicializacion -----------------

void Game::cargarMundo() {
    auto mapasConfig = config.getMapas();
    for (const auto& cm : mapasConfig) {
        auto mapa = std::make_unique<Mapa>(cm.ancho, cm.alto, cm.esZonaSegura);
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

void Game::cargarJugadoresPersistidos() {
    auto players = PersistenceLoader::load_players(config.getRutaJugadores());

    for (const auto& p : players) {
        bool ok = agregarJugador(p.nick, p.mapa_id, p.x, p.y, p.raza, p.clase);
        if (!ok) continue;

        Jugador* jugador = getJugador(p.nick);
        if (!jugador) continue;

        jugador->setDireccion(static_cast<Direccion>(p.direction));
        jugador->restaurarEstado(p.nivel, p.vida, p.vida_max, p.mana,p.mana_max,
                                p.experiencia, p.oro,
                                p.constitucion, p.inteligencia, p.fuerza, p.agilidad
        );
        // TODO: restaurar inventario
    }
}

void Game::cargarSacerdotes() {
    for (const auto& cm : config.getMapas()) {
        for (const auto& pos : cm.sacerdotes) {
            sacerdotes.push_back({cm.id, pos.x, pos.y});
        }
    }
}

// ----------------- Helpers internos -----------------

std::string Game::getNombreJugadorPorComando(const Command& cmd) const {
    auto it = player_id_to_nick.find(cmd.get_player_id());
    return (it != player_id_to_nick.end()) ? it->second : "";
}

bool Game::handle_meditation_interruption(Jugador* jugador,
                                            std::vector<Snapshot>& snapshots,
                                            const std::string& nombre) {
    if (!jugador || !jugador->estaMeditando()) return false;

    jugador->interrumpirMeditacion();
    snapshots.push_back(Snapshot::meditation_status(nombre, false));
    snapshots.push_back(make_player_stats(jugador));
    return true;
}

static Snapshot make_player_stats(Jugador* j) {
    return Snapshot::player_stats(
        j->getNombre(), j->getRaza()->getNombre(), j->getClase()->getNombre(),
        static_cast<uint16_t>(j->getMapaId()),
        static_cast<uint16_t>(j->getPosX()),
        static_cast<uint16_t>(j->getPosY()),
        static_cast<uint8_t>(j->getDireccion()),
        static_cast<uint16_t>(j->getNivel()),
        static_cast<uint16_t>(j->getVidaActual()),
        static_cast<uint16_t>(j->getVidaMax()),
        static_cast<uint16_t>(j->getManaActual()),
        static_cast<uint16_t>(j->getManaMax()),
        static_cast<uint32_t>(j->getExperiencia()),
        static_cast<uint32_t>(j->getOro()),
        static_cast<uint16_t>(j->getConstitucion()),
        static_cast<uint16_t>(j->getInteligencia()),
        static_cast<uint16_t>(j->getFuerza()),
        static_cast<uint16_t>(j->getAgilidad()));
}

// ----------------- Gestion de jugadores -----------------

bool Game::agregarJugador(const std::string& nombre, int mapaId, int posX, int posY,
                           const std::string& razaNombre,
                           const std::string& claseNombre) {
    if (jugadores.count(nombre)) return false;
 
    auto itRaza  = razas.find(razaNombre);
    auto itClase = clases.find(claseNombre);
    if (itRaza == razas.end() || itClase == clases.end()) return false;
 
    auto jugador = std::make_unique<Jugador>(nombre, posX, posY,
                                              itRaza->second.get(),
                                              itClase->second.get(),
                                              config.getInventarioCapacidadMax());
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

std::string Game::agregarCriatura(const std::string& tipo, int mapaId, int posX, int posY) {
    std::string id = tipo + "_" + std::to_string(nextCriaturaId++);

    std::unique_ptr<Criatura> criatura;
    if      (tipo == "goblin")      criatura = std::make_unique<Goblin>(config, posX, posY);
    else if (tipo == "esqueleto")   criatura = std::make_unique<Esqueleto>(config, posX, posY);
    else if (tipo == "arana")       criatura = std::make_unique<Arana>(config, posX, posY);
    else if (tipo == "golem")       criatura = std::make_unique<Golem>(config, posX, posY);
    else if (tipo == "orco")        criatura = std::make_unique<Orco>(config, posX, posY);
    else if (tipo == "zombie")      criatura = std::make_unique<Zombie>(config, posX, posY);
    else return "";

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

// ----------------- Combate -----------------

bool Game::puedeAtacarJugador(Jugador* atacante, Jugador* objetivo) {
    if (atacante->getNivel() <= 12 || objetivo->getNivel() <= 12) return false;
    if (std::abs(atacante->getNivel() - objetivo->getNivel()) > 10) return false;

    // Zona segura
    Mapa* mapa = mundo.getMapa(atacante->getMapaId());
    if (mapa && mapa->esZonaSegura()) return false;

    return true;
}

static std::unique_ptr<Item> crearItemAleatorio() {
    using Fn = std::unique_ptr<Item>(*)();
    static const Fn items[] = {
        []() -> std::unique_ptr<Item> { return ItemFactory::crearEspada(); },
        []() -> std::unique_ptr<Item> { return ItemFactory::crearHacha(); },
        []() -> std::unique_ptr<Item> { return ItemFactory::crearMartillo(); },
        []() -> std::unique_ptr<Item> { return ItemFactory::crearArcoSimple(); },
        []() -> std::unique_ptr<Item> { return ItemFactory::crearArcoCompuesto(); },
        []() -> std::unique_ptr<Item> { return ItemFactory::crearVaraDeFresno(); },
        []() -> std::unique_ptr<Item> { return ItemFactory::crearBaculoNudoso(); },
        []() -> std::unique_ptr<Item> { return ItemFactory::crearBaculoEngarzado(); },
        []() -> std::unique_ptr<Item> { return ItemFactory::crearArmaduraDeCuero(); },
        []() -> std::unique_ptr<Item> { return ItemFactory::crearArmaduraDePlacas(); },
        []() -> std::unique_ptr<Item> { return ItemFactory::crearTunicaAzul(); },
        []() -> std::unique_ptr<Item> { return ItemFactory::crearCapucha(); },
        []() -> std::unique_ptr<Item> { return ItemFactory::crearCascoDeHierro(); },
        []() -> std::unique_ptr<Item> { return ItemFactory::crearSombreroMagico(); },
        []() -> std::unique_ptr<Item> { return ItemFactory::crearEscudoDeTortuga(); },
        []() -> std::unique_ptr<Item> { return ItemFactory::crearEscudoDeHierro(); },
    };
    int idx = rand() % (sizeof(items) / sizeof(items[0]));
    return items[idx]();
}

void Game::procesarDropCriatura(Jugador* atacante, Criatura* criatura) {
    double r = static_cast<double>(rand()) / RAND_MAX;

    if (r < 0.90) return;   // 90% de no dropear nada

    if (r < 0.98) {         // 8% de dropear oro
        atacante->agregarOro(Formulas::calcularOroDropNPC(criatura->getVidaMax()));
        return;
    }

    if (r < 0.99) {         // 1% de dropear pocion aleatoria
        bool esVida = rand() % 2 == 0;
        auto pocion = esVida ? ItemFactory::crearPocionDeVida() : ItemFactory::crearPocionDeMana();
        atacante->agarrarItem(std::move(pocion));
        return;
    }

                            // 1% de dropear item aleatorio
    atacante->agarrarItem(crearItemAleatorio());
}

ResultadoAtaque Game::atacarCriatura(Jugador* atacante, Criatura* objetivo) {
    ResultadoAtaque resultado{false, 0, false, false, false};

    if (!atacante->estaVivo() || !objetivo->estaVivo()) return resultado;
    resultado.exito = true;

    int fuerza = atacante->getFuerza();
    const Arma* arma = atacante->getInventario().getArmaEquipada();
    const Baculo* baculo = atacante->getInventario().getBaculoEquipado();

    bool esAtaqueDeRango = (arma && arma->esDeRango()) || (baculo != nullptr);
    if (!esAtaqueDeRango) {
        int dx = std::abs(atacante->getPosX() - objetivo->getPosX());
        int dy = std::abs(atacante->getPosY() - objetivo->getPosY());
        if (dx + dy != 1) return resultado;
    }

    if (!arma && !baculo) return resultado;

    int danio = arma
        ? Formulas::calcularDanio(fuerza, arma->getDanioMin(), arma->getDanioMax())
        : Formulas::calcularDanio(fuerza, baculo->getEfectoMin(), baculo->getEfectoMax());
    
    resultado.fueCritico = Formulas::calcularCritico();
    if (resultado.fueCritico) danio *= 2;

    int danioFinal = danio;
    resultado.danioAplicado = danioFinal;
    objetivo->recibirDanio(danioFinal);

    atacante->ganarExperiencia(Formulas::calcularExpAtaque(danioFinal, objetivo->getNivel(), atacante->getNivel()));

    resultado.objetivoMurio = !objetivo->estaVivo();
    if (resultado.objetivoMurio) {
        atacante->ganarExperiencia(
            Formulas::calcularExpMatar(objetivo->getVidaMax(), atacante->getNivel(),
                                        atacante->getNivel()));

        procesarDropCriatura(atacante, objetivo);
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
    resultado.exito = true;

    int fuerza = atacante->getFuerza();
    const Arma* arma = atacante->getInventario().getArmaEquipada();
    const Baculo* baculo = atacante->getInventario().getBaculoEquipado();

    bool esAtaqueDeRango = (arma && arma->esDeRango()) || (baculo != nullptr);
    if (!esAtaqueDeRango) {
        int dx = std::abs(atacante->getPosX() - objetivo->getPosX());
        int dy = std::abs(atacante->getPosY() - objetivo->getPosY());
        if (dx + dy != 1) return resultado;   // no adyacente
    }

    if (!arma && !baculo) return resultado;

    int danio = arma
        ? Formulas::calcularDanio(fuerza, arma->getDanioMin(), arma->getDanioMax())
        : Formulas::calcularDanio(fuerza, baculo->getEfectoMin(), baculo->getEfectoMax());

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

        int oroExceso = Formulas::calcularOroExceso(objetivo->getOro(), objetivo->getOroMax());
        if (oroExceso > 0) {
            objetivo->gastarOro(oroExceso);
            atacante->agregarOro(oroExceso);
        }
    }

    return resultado;
}

// ----------------- Tick -----------------

std::vector<Snapshot> Game::tick(float dt) {
    std::vector<Snapshot> snapshots;

    for (auto& [nombre, jugador] : jugadores) {
        bool wasMeditating = jugador->estaMeditando();
        int oldMana = jugador->getManaActual();

        jugador->recuperacionPasiva(dt);
        // por ahora mando todo, pero en un futuro podria optimizarse para mandar solo cambios relevantes
        if (wasMeditating && jugador->getManaActual() != oldMana)
            snapshots.push_back(make_player_stats(jugador.get()));
    }

    tickCriaturas(dt, snapshots);
    tickResucitando(dt, snapshots);

    tiempoDesdeUltimoSpawn += dt;
    if (tiempoDesdeUltimoSpawn >= config.getSpawnIntervalo()) {
        spawnCriaturas();
        tiempoDesdeUltimoSpawn = 0.0f;
    }

    return snapshots;
}

// ----------------- process() -----------------

// TODO : mandar snapshots que tengan sentido con cada accion
std::vector<Snapshot> Game::process(const Command& cmd) {
    std::vector<Snapshot> snapshots;

    // -- Login --------------------------
    if (cmd.get_type() == protocol::ClientOpcode::LOGIN) {
        Jugador* jugador = getJugador(cmd.get_nick());

        if (!jugador) {
            snapshots.push_back(Snapshot::error_message(
                    cmd.get_nick(), "Login fallido: personaje inexistente"));
            return snapshots;
        }
        player_id_to_nick[cmd.get_player_id()] = cmd.get_nick();

        snapshots.push_back(Snapshot::entity_login(
                cmd.get_nick(),
                static_cast<uint16_t>(jugador->getPosX()),
                static_cast<uint16_t>(jugador->getPosY()),
                static_cast<uint8_t>(jugador->getDireccion())
            )
        );

        return snapshots;
    }

    // -- Create Character --------------------------
    if (cmd.get_type() == protocol::ClientOpcode::CREATE_CHARACTER) {
        bool creado = agregarJugador(cmd.get_nick(), 1, 10, 10,
                                      cmd.get_raza(), cmd.get_clase());
        if (!creado) {
            snapshots.push_back(Snapshot::error_message(
                cmd.get_nick(), "No se pudo crear el personaje"));
            return snapshots;
        }
        player_id_to_nick[cmd.get_player_id()] = cmd.get_nick();

        Jugador* j = getJugador(cmd.get_nick());
        snapshots.push_back(Snapshot::entity_created(
            cmd.get_nick(),
            static_cast<uint16_t>(j->getPosX()),
            static_cast<uint16_t>(j->getPosY()),
            static_cast<uint8_t>(j->getDireccion())));
        return snapshots;
    }

    // -- Disconnect --------------------------
    if (cmd.is_disconnect()) {
        const std::string nombre = getNombreJugadorPorComando(cmd);
        if (!nombre.empty()) {
            player_id_to_nick.erase(cmd.get_player_id());
            snapshots.push_back(Snapshot::entity_remove(nombre));
        }
        return snapshots;
    }

    // -- Comando que requieren jugador --------------------------
    const std::string nombre = getNombreJugadorPorComando(cmd);
    if (nombre.empty()) {
        snapshots.push_back(Snapshot::error_message(
            "", "Comando recibido sin jugador asociado"));
        return snapshots;
    }
 
    Jugador* jugador = getJugador(nombre);

    // Comandos que un fantasma (vivo == false) NO puede ejecutar
    static const std::initializer_list<protocol::ClientOpcode> bloqueadosParaFantasma = {
        protocol::ClientOpcode::ATTACK,
        protocol::ClientOpcode::MEDITATE,
        protocol::ClientOpcode::EQUIP_ITEM,
        protocol::ClientOpcode::PICK_ITEM,
        protocol::ClientOpcode::DROP_ITEM,
        protocol::ClientOpcode::BUY_ITEM,
        protocol::ClientOpcode::SELL_ITEM,
        protocol::ClientOpcode::DEPOSIT_ITEM,
        protocol::ClientOpcode::WITHDRAW_ITEM,
    };
    if (jugador && !jugador->estaVivo()) {
        for (auto op : bloqueadosParaFantasma) {
            if (cmd.get_type() == op) {
                snapshots.push_back(Snapshot::error_message(nombre,
                    "No puedes hacer eso siendo un fantasma"));
                return snapshots;
            }
        }
    }

    // Deja de meditar si recibe cualquier comando distinto a MEDITATE mientras se está meditando
    if (jugador && cmd.get_type() != protocol::ClientOpcode::MEDITATE) {
        handle_meditation_interruption(jugador, snapshots, nombre);
    }

    switch (cmd.get_type()) {

        // -- MOVE --------------------------
        case protocol::ClientOpcode::MOVE: {
            bool moved = moverJugador(nombre,
                static_cast<Direccion>(cmd.get_direction()));
            if (moved) {
                Jugador* j = getJugador(nombre);
                snapshots.push_back(Snapshot::entity_move(
                    nombre,
                    static_cast<uint16_t>(j->getPosX()),
                    static_cast<uint16_t>(j->getPosY()),
                    static_cast<uint8_t>(j->getDireccion())));
            } else {
                snapshots.push_back(Snapshot::error_message(nombre, "No se pudo mover"));
            }
            break;
        }

        // -- PICK ITEM --------------------------
        case protocol::ClientOpcode::PICK_ITEM: {
            if (tomarItem(nombre, 0))
                snapshots.push_back(Snapshot::error_message(nombre, "Item recogido"));
            else
                snapshots.push_back(Snapshot::error_message(nombre, "No hay item para recoger"));
            break;
        }

        // -- DROP ITEM --------------------------
        case protocol::ClientOpcode::DROP_ITEM: {
            if (tirarItem(nombre, cmd.get_item_id()))
                snapshots.push_back(Snapshot::error_message(nombre, "Item arrojado"));
            else
                snapshots.push_back(Snapshot::error_message(nombre, "No se pudo arrojar el item"));
            break;
        }

        // -- ATTACK --------------------------
        case protocol::ClientOpcode::ATTACK: {
            const std::string objetivo = cmd.get_nick();
            ResultadoAtaque resultado = atacar(nombre, objetivo);
 
            if (!resultado.exito) {
                snapshots.push_back(Snapshot::error_message(nombre, "Ataque invalido"));
                break;
            }
            if (resultado.fueEsquivado) {
                snapshots.push_back(Snapshot::dodge_event(nombre, objetivo));
                break;
            }
            snapshots.push_back(Snapshot::damage_event(
                nombre, objetivo,
                static_cast<uint16_t>(resultado.danioAplicado),
                resultado.fueCritico));
 
            if (resultado.objetivoMurio) {
                snapshots.push_back(Snapshot::death_event(objetivo));
                // Si era criatura, removerla del mundo
                if (getCriatura(objetivo)) removerCriatura(objetivo);
                else                       snapshots.push_back(Snapshot::entity_remove(objetivo));
            }
            break;
        }

        // -- MEDITATE --------------------------
        case protocol::ClientOpcode::MEDITATE: {
            if (!jugador || !jugador->estaVivo()) {
                snapshots.push_back(Snapshot::error_message(nombre,
                    "No se puede meditar si no estas vivo"));
                break;
            }
            if (!jugador->getClase()->puedeMeditar()) {
                snapshots.push_back(Snapshot::error_message(nombre,
                    "Tu clase no puede meditar"));
                break;
            }
            if (jugador->estaMeditando()) {
                snapshots.push_back(Snapshot::error_message(nombre, "Ya estas meditando"));
                break;
            }
            jugador->iniciarMeditacion();
            snapshots.push_back(Snapshot::meditation_status(nombre, true));
            snapshots.push_back(make_player_stats(jugador));
            break;
        }

        // -- EQUIP ITEM --------------------------
        case protocol::ClientOpcode::EQUIP_ITEM: {
            if (!jugador || !jugador->estaVivo()) {
                snapshots.push_back(Snapshot::error_message(nombre,
                    "No puedes equipar items si no estas vivo"));
                break;
            }
            int slot = static_cast<int>(cmd.get_item_id());
            const auto& slots = jugador->getInventario().getSlots();
            if (slot < 0 || slot >= (int)slots.size()) {
                snapshots.push_back(Snapshot::error_message(nombre,
                    "Slot de inventario invalido"));
                break;
            }
            bool ok = false;
            switch (slots[slot].item->getTipo()) {
                case TipoItem::ARMA:     ok = jugador->equiparArma(slot);     break;
                case TipoItem::BACULO:   ok = jugador->equiparBaculo(slot);   break;
                case TipoItem::ARMADURA: ok = jugador->equiparArmadura(slot); break;
                case TipoItem::CASCO:    ok = jugador->equiparCasco(slot);    break;
                case TipoItem::ESCUDO:   ok = jugador->equiparEscudo(slot);   break;
                case TipoItem::POCION:   ok = jugador->usarPocion(slot);      break;
                default: break;
            }
            if (!ok)
                snapshots.push_back(Snapshot::error_message(nombre, "No se pudo usar/equipar el item"));
            else
                snapshots.push_back(make_player_stats(jugador));
            break;
        }

/*
        // -- CHEATS --------------------------
        case protocol::ClientOpcode::CHEAT_GOD: {
            if (jugador) jugador->activarCheatVidaInfinita();
            snapshots.push_back(Snapshot::error_message(nombre, "Cheat: vida infinita activado"));
            break;
        }
        case protocol::ClientOpcode::CHEAT_MANA: {
            if (jugador) jugador->activarCheatManaInfinito();
            snapshots.push_back(Snapshot::error_message(nombre, "Cheat: mana infinito activado"));
            break;
        }
        case protocol::ClientOpcode::CHEAT_DIE: {
            if (jugador) jugador->morir();
            snapshots.push_back(Snapshot::death_event(nombre));
            snapshots.push_back(Snapshot::entity_remove(nombre));
            break;
        }
        case protocol::ClientOpcode::CHEAT_RESURRECT: {
            if (jugador) jugador->revivir(jugador->getVidaMax());
            snapshots.push_back(Snapshot::error_message(nombre, "Cheat: resucitado"));
            break;
        }
*/  

        // -- RESURRECT --------------------------
        case protocol::ClientOpcode::RESURRECT: {
            if (!jugador || jugador->estaVivo()) {
                snapshots.push_back(Snapshot::error_message(nombre,
                    "Solo un fantasma puede usar /resucitar"));
                break;
            }
            if (jugador->estaResucitando()) {
                snapshots.push_back(Snapshot::error_message(nombre,
                    "Ya estas resucitando"));
                break;
            }
            InfoSacerdote destino;
            float distancia;
            if (!encontrarSacerdoteMasCercano(jugador, destino, distancia)) {
                snapshots.push_back(Snapshot::error_message(nombre,
                    "No hay ningun sacerdote en el mundo"));
                break;
            }
            float tiempo = distancia / config.getVelocidadResurreccion();
            jugador->iniciarResurreccion(tiempo, destino.mapaId, destino.x, destino.y);
            snapshots.push_back(Snapshot::error_message(nombre,
                "Resucitando... quedas inmovilizado hasta haber resucitado"));
            break;
        }

        // -- HEAL --------------------------
        case protocol::ClientOpcode::HEAL: {
            if (!jugador || !jugador->estaVivo()) {
                snapshots.push_back(Snapshot::error_message(nombre,
                    "No puedes curarte si eres un fantasma"));
                break;
            }
            // Verificar que haya un sacerdote cercano en el mismo mapa
            bool sacerdoteCercano = false;
            for (const auto& s : sacerdotes) {
                if (s.mapaId != jugador->getMapaId()) continue;
                int dx = std::abs(s.x - jugador->getPosX());
                int dy = std::abs(s.y - jugador->getPosY());
                if (dx + dy <= 10) { sacerdoteCercano = true; break; }
            }
            if (!sacerdoteCercano) {
                snapshots.push_back(Snapshot::error_message(nombre,
                    "Debes estar cerca de un sacerdote para curarte"));
                break;
            }
            jugador->curar(jugador->getVidaMax());
            jugador->recuperarMana(jugador->getManaMax());
            snapshots.push_back(make_player_stats(jugador));
            break;
        }

        // -- Comandos con NPCs (pendientes) --------------------------
        case protocol::ClientOpcode::BUY_ITEM:
            snapshots.push_back(Snapshot::error_message(nombre,
                "Compra en comercio no implementada"));
            break;

        case protocol::ClientOpcode::SELL_ITEM:
            snapshots.push_back(Snapshot::error_message(nombre,
                "Venta en comercio no implementada"));
            break;

        case protocol::ClientOpcode::DEPOSIT_ITEM:
            snapshots.push_back(Snapshot::error_message(nombre,
                "Deposito en banco no implementado"));
            break;

        case protocol::ClientOpcode::WITHDRAW_ITEM:
            snapshots.push_back(Snapshot::error_message(nombre,
                "Extraccion de banco no implementada"));
            break;

        // -- Chat privado --------------------------
        case protocol::ClientOpcode::PRIVATE_MESSAGE: {
            const std::string destino = cmd.get_nick();
            const std::string mensaje = cmd.get_text();
            if (destino.empty()) {
                snapshots.push_back(Snapshot::error_message(nombre,
                    "Debe indicar un destinatario"));
                break;
            }

            if (mensaje.empty()) {
                snapshots.push_back(Snapshot::error_message(nombre,
                    "El mensaje no puede estar vacio"));
                break;
            }

            if (!getJugador(destino)) {
                snapshots.push_back(Snapshot::error_message(nombre,
                    "El jugador destinatario no existe"));
                break;
            }

            snapshots.push_back(Snapshot::chat_message(nombre, destino, mensaje));
            break;
        }

        // -- Clanes (pendientes) --------------------------
        case protocol::ClientOpcode::CLAN_CREATE:
        case protocol::ClientOpcode::CLAN_JOIN:
        case protocol::ClientOpcode::CLAN_REVIEW:
        case protocol::ClientOpcode::CLAN_ACCEPT:
        case protocol::ClientOpcode::CLAN_REJECT:
        case protocol::ClientOpcode::CLAN_BAN:
        case protocol::ClientOpcode::CLAN_KICK:
        case protocol::ClientOpcode::CLAN_LEAVE:
            snapshots.push_back(Snapshot::error_message(nombre,
                "Clanes todavia no implementados"));
            break;
 
        default:
            snapshots.push_back(Snapshot::error_message(nombre,
                "Comando no implementado"));
            break;
    }

    return snapshots;
}

// ----------------- Resto de metodos -----------------

const Mundo& Game::getMundo() const { return mundo; }

void Game::spawnCriaturas() {
    for (auto& [mapaId, info] : infoSpawn) {
        if (info.criaturasPosibles.empty()) continue;

        int poblacionActual = 0;
        for (auto& [id, c] : criaturas) {
            if (c->getMapaId() == mapaId && c->estaVivo())
                poblacionActual++;
        }

        if (poblacionActual >= info.poblacionMax) continue;

        Mapa* mapa = mundo.getMapa(mapaId);
        if (!mapa) continue;
        if (mapa->esZonaSegura()) continue;

        const std::string& tipo = info.criaturasPosibles[rand() % info.criaturasPosibles.size()];

        for (int intento = 0; intento < 20; intento++) {
            int x = rand() % mapa->getAncho();
            int y = rand() % mapa->getAlto();
            if (mapa->esTransitable(x, y) && !mapa->estaOcupada(x, y)) {
                agregarCriatura(tipo, mapaId, x, y);
                break;
            }
        }
    }
}

int Game::criaturaAtacaJugador(Criatura* atacante, Jugador* objetivo) {
    if (!atacante->estaVivo() || !objetivo->estaVivo()) return 0;
    int danio = Formulas::calcularDanio(atacante->getFuerza(),
                                        atacante->getDanioMin(),
                                        atacante->getDanioMax());
    objetivo->recibirDanio(danio);
    return danio;
}

void Game::tickCriaturas(float dt, std::vector<Snapshot>& snapshots) {
    int rango = config.getCriaturaRangoDeteccion();
    float cdAtq = config.getCriaturaCooldownAtaque();
    float cdMov = config.getCriaturaCooldownMovimiento();

    std::vector<std::string> criaturasMuertas;

    for (auto& [id, criatura] : criaturas) {
        if (!criatura->estaVivo()) continue;

        criatura->actualizarTiempoAtaque(dt);
        criatura->actualizarTiempoMovimiento(dt);

        Jugador* objetivo = nullptr;
        double distanciaMin = static_cast<double>(rango) + 1.0;

        for (auto& [nombre, jugador] : jugadores) {
            if (!jugador->estaVivo()) continue;
            if (jugador->getMapaId() != criatura->getMapaId()) continue;

            double dx = jugador->getPosX() - criatura->getPosX();
            double dy = jugador->getPosY() - criatura->getPosY();
            double distancia = std::sqrt(dx*dx + dy*dy);

            if (distancia <= rango && distancia < distanciaMin) {
                distanciaMin = distancia;
                objetivo = jugador.get();
            }
        }

        if (!objetivo) continue;

        int dx = objetivo->getPosX() - criatura->getPosX();
        int dy = objetivo->getPosY() - criatura->getPosY();
        bool adyacente = (std::abs(dx) + std::abs(dy)) == 1;

        // Atacar si está adyacente y el cooldown se cumplió
        if (adyacente && criatura->getTiempoDesdeUltimoAtaque() >= cdAtq) {
            int danio = criaturaAtacaJugador(criatura.get(), objetivo);
            criatura->resetearCooldownAtaque();

            snapshots.push_back(Snapshot::damage_event(
                id,
                objetivo->getNombre(),
                static_cast<uint16_t>(danio),
                false));

            if (!objetivo->estaVivo()) {
                snapshots.push_back(Snapshot::death_event(objetivo->getNombre()));
                snapshots.push_back(Snapshot::entity_remove(objetivo->getNombre()));
            }
            continue;
        }

        // Moverse hacia el objetivo si no está adyacente y el cooldown se cumplió
        if (!adyacente && criatura->getTiempoDesdeUltimoMovimiento() >= cdMov) {
            Direccion dir;
            if (std::abs(dx) >= std::abs(dy)) {
                dir = (dx > 0) ? Direccion::ESTE : Direccion::OESTE;
            } else {
                dir = (dy > 0) ? Direccion::SUR : Direccion::NORTE;
            }
            mundo.moverPersonaje(criatura.get(), dir);
            criatura->resetearCooldownMovimiento();

            snapshots.push_back(Snapshot::entity_move(
                id,
                static_cast<uint16_t>(criatura->getPosX()),
                static_cast<uint16_t>(criatura->getPosY()),
                static_cast<uint8_t>(criatura->getDireccion())));
        }  
    }    
}

bool Game::encontrarSacerdoteMasCercano(const Jugador* fantasma,
                                        InfoSacerdote& destino,
                                        float& distancia) const {
    bool encontrado = false;
    float distMin = std::numeric_limits<float>::max();

    for (const auto& s : sacerdotes) {
        float dx = static_cast<float>(s.x - fantasma->getPosX());
        float dy = static_cast<float>(s.y - fantasma->getPosY());
        float dist = std::sqrt(dx*dx + dy*dy);
        if (dist < distMin) {
            distMin = dist;
            destino = s;
            encontrado = true;
        }
    }

    distancia = distMin;
    return encontrado;
}

void Game::tickResucitando(float dt, std::vector<Snapshot>& snapshots) {
    for (auto& [nombre, jugador] : jugadores) {
        if (!jugador->estaResucitando()) continue;
 
        jugador->tickResurreccion(dt);
 
        if (jugador->resurreccionCompleta()) {
            mundo.removerPersonaje(jugador.get());
            jugador->setPosicion(jugador->getDestinoPosX(), jugador->getDestinoPosY());
            jugador->setMapaId(jugador->getDestinoMapaId());
            mundo.agregarPersonaje(jugador.get());
            jugador->revivir(jugador->getVidaMax());
 
            snapshots.push_back(Snapshot::entity_move(
                nombre,
                static_cast<uint16_t>(jugador->getPosX()),
                static_cast<uint16_t>(jugador->getPosY()),
                static_cast<uint8_t>(jugador->getDireccion())));
            snapshots.push_back(make_player_stats(jugador.get()));
        }
    }
}
 
bool Game::tirarItem(const std::string& nombre, int indice, int cantidad) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador || !jugador->estaVivo()) return false;
    auto slot = jugador->soltarItem(indice, cantidad);
    if (!slot) return false;
    mundo.tirarItem(jugador->getMapaId(),
                    jugador->getPosX(), jugador->getPosY(),
                    std::move(*slot));
    return true;
}
 
bool Game::tomarItem(const std::string& nombre, int indice) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador || !jugador->estaVivo()) return false;
    if (jugador->getInventario().estaLleno()) return false;
    auto slot = mundo.tomarItemEnPosicion(jugador->getMapaId(),
                                           jugador->getPosX(),
                                           jugador->getPosY(), indice);
    if (!slot) return false;
    jugador->agarrarItem(std::move(slot->item), slot->cantidad);
    return true;
}
 
std::vector<PersistenceTask>
Game::build_persistence_tasks_for_command(const Command& cmd) const {
    std::vector<PersistenceTask> tasks;
    const std::string actor = getNombreJugadorPorComando(cmd);
    for (const std::string& name :
         PersistenceTaskFactory::get_affected_players(cmd, actor)) {
        const Jugador* j = getJugador(name);
        if (j) tasks.push_back(PersistenceTaskFactory::from_player(*j));
    }
    return tasks;
}