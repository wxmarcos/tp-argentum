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

    auto it =
        player_id_to_nick.find(
            cmd.get_player_id());

    if (it == player_id_to_nick.end()) {
        return "";
    }

    return it->second;
}
// TODO : mandar snapshots que tengan sentido con cada accion
std::vector<Snapshot> Game::process(const Command& cmd) {
    std::vector<Snapshot> snapshots;

    if (cmd.get_type() == protocol::ClientOpcode::LOGIN) {
        Jugador* jugador = getJugador(cmd.get_nick());

        if (!jugador) {
            snapshots.push_back(
                Snapshot::error_message(
                    cmd.get_nick(),
                    "Login fallido: personaje inexistente"
                )
            );
            return snapshots;
        }

        player_id_to_nick[cmd.get_player_id()] = cmd.get_nick();

        snapshots.push_back(
            Snapshot::entity_login(
                cmd.get_nick(),
                static_cast<uint16_t>(jugador->getPosX()),
                static_cast<uint16_t>(jugador->getPosY()),
                static_cast<uint8_t>(jugador->getDireccion())
            )
        );

        return snapshots;
    }

    if (cmd.get_type() == protocol::ClientOpcode::CREATE_CHARACTER) {
        bool creado = agregarJugador(
            cmd.get_nick(),
            1,
            10,
            10,
            cmd.get_raza(),
            cmd.get_clase()
        );

        if (!creado) {
            snapshots.push_back(
                Snapshot::error_message(
                    cmd.get_nick(),
                    "No se pudo crear el personaje"
                )
            );
            return snapshots;
        }

        player_id_to_nick[cmd.get_player_id()] = cmd.get_nick();

        snapshots.push_back(
            Snapshot::entity_created(
                cmd.get_nick(),
                static_cast<uint16_t>(getJugador(cmd.get_nick())->getPosX()),
                static_cast<uint16_t>(getJugador(cmd.get_nick())->getPosY()),
                static_cast<uint8_t>(getJugador(cmd.get_nick())->getDireccion())
            )
        );

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

    if (nombre.empty()) {
        snapshots.push_back(
            Snapshot::error_message(
                "",
                "Comando recibido sin jugador asociado"
            )
        );
        return snapshots;
    }

    Jugador* jugador = getJugador(nombre);
    // Deja de meditar si recibe cualquier comando distinto a MEDITATE mientras se está meditando
    if (jugador && cmd.get_type() != protocol::ClientOpcode::MEDITATE) {
        handle_meditation_interruption(jugador, snapshots, nombre);
    }

    switch (cmd.get_type()) {
        case protocol::ClientOpcode::MOVE: {
            bool moved = moverJugador(
                nombre,
                static_cast<Direccion>(cmd.get_direction())
            );

            if (moved) {
                Jugador* jugador = getJugador(nombre);
                snapshots.push_back(
                    Snapshot::entity_move(
                        nombre,
                        static_cast<uint16_t>(jugador->getPosX()),
                        static_cast<uint16_t>(jugador->getPosY()),
                        static_cast<uint8_t>(jugador->getDireccion())
                    )
                );
            } else {
                snapshots.push_back(
                    Snapshot::error_message(
                        nombre,
                        "No se pudo mover"
                    )
                );
            }

            break;
        }

        case protocol::ClientOpcode::PICK_ITEM: {
            if (tomarItem(nombre, 0)) {
                snapshots.push_back(
                    Snapshot::error_message(
                        nombre,
                        "Item recogido"
                    )
                );
            } else {
                snapshots.push_back(
                    Snapshot::error_message(
                        nombre,
                        "No hay item para recoger"
                    )
                );
            }

            break;
        }

        case protocol::ClientOpcode::DROP_ITEM: {
            if (tirarItem(nombre, cmd.get_item_id())) {
                snapshots.push_back(
                    Snapshot::error_message(
                        nombre,
                        "Item arrojado"
                    )
                );
            } else {
                snapshots.push_back(
                    Snapshot::error_message(
                        nombre,
                        "No se pudo arrojar el item"
                    )
                );
            }

            break;
        }

        case protocol::ClientOpcode::ATTACK: {
            const std::string objetivo = cmd.get_nick();

            ResultadoAtaque resultado = atacar(nombre, objetivo);

            if (!resultado.exito) {
                snapshots.push_back(
                    Snapshot::error_message(
                        nombre,
                        "Ataque invalido"
                    )
                );
                break;
            }

            if (resultado.fueEsquivado) {
                snapshots.push_back(
                    Snapshot::dodge_event(nombre, objetivo)
                );
                break;
            }

            snapshots.push_back(
                Snapshot::damage_event(
                    nombre,
                    objetivo,
                    static_cast<uint16_t>(resultado.danioAplicado),
                    resultado.fueCritico
                )
            );

            if (resultado.objetivoMurio) {
                snapshots.push_back(
                    Snapshot::death_event(objetivo)
                );

                snapshots.push_back(
                    Snapshot::entity_remove(objetivo)
                );
            }

            break;
        }

        case protocol::ClientOpcode::MEDITATE: {
            if (!jugador || !jugador->estaVivo()) {
                snapshots.push_back(
                    Snapshot::error_message(
                        nombre,
                        "No se puede meditar si no estas vivo"
                    )
                );
                break;
            }

            if (!jugador->getClase()->puedeMeditar()) {
                snapshots.push_back(
                    Snapshot::error_message(
                        nombre,
                        "Tu clase no puede meditar"
                    )
                );
                break;
            }

            if (jugador->estaMeditando()) {
                snapshots.push_back(
                    Snapshot::error_message(
                        nombre,
                        "Ya estas meditando"
                    )
                );
                break;
            }

            jugador->iniciarMeditacion();
            snapshots.push_back(
                Snapshot::meditation_status(nombre, true)
            );
            snapshots.push_back(
                Snapshot::player_stats(
                    jugador->getNombre(),
                    jugador->getRaza()->getNombre(),
                    jugador->getClase()->getNombre(),
                    static_cast<uint16_t>(jugador->getMapaId()),
                    static_cast<uint16_t>(jugador->getPosX()),
                    static_cast<uint16_t>(jugador->getPosY()),
                    static_cast<uint8_t>(jugador->getDireccion()),
                    static_cast<uint16_t>(jugador->getNivel()),
                    static_cast<uint16_t>(jugador->getVidaActual()),
                    static_cast<uint16_t>(jugador->getVidaMax()),
                    static_cast<uint16_t>(jugador->getManaActual()),
                    static_cast<uint16_t>(jugador->getManaMax()),
                    static_cast<uint32_t>(jugador->getExperiencia()),
                    static_cast<uint32_t>(jugador->getOro()),
                    static_cast<uint16_t>(jugador->getConstitucion()),
                    static_cast<uint16_t>(jugador->getInteligencia()),
                    static_cast<uint16_t>(jugador->getFuerza()),
                    static_cast<uint16_t>(jugador->getAgilidad())
                )
            );
            break;
        }

        case protocol::ClientOpcode::RESURRECT: {
            // Resurrección por comando no soportada mientras no haya NPCs
            snapshots.push_back(
                Snapshot::error_message(
                    nombre,
                    "Resucitar por comando no implementado.  a un Sacerdote en la ciudad."
                )
            );
            break;
        }

        case protocol::ClientOpcode::HEAL: {
            // Curación por comando no soportada mientras no haya NPCs
            snapshots.push_back(
                Snapshot::error_message(
                    nombre,
                    "Curar por comando no implementado. Acercate a un Sacerdote."
                )
            );
            break;
            break;
        }

        case protocol::ClientOpcode::EQUIP_ITEM: {
            Jugador* jugador = getJugador(nombre);
            if (!jugador || !jugador->estaVivo()) {
                snapshots.push_back(
                    Snapshot::error_message(
                        nombre,
                        "No puedes equipar items si no estas vivo"
                    )
                );
                break;
            }

            int slot = static_cast<int>(cmd.get_item_id());
            const auto& slots = jugador->getInventario().getSlots();
            if (slot < 0 || slot >= (int)slots.size()) {
                snapshots.push_back(
                    Snapshot::error_message(
                        nombre,
                        "Slot de inventario invalido"
                    )
                );
                break;
            }

            bool equipped = false;
            switch (slots[slot].item->getTipo()) {
                case TipoItem::ARMA:
                    equipped = jugador->equiparArma(slot);
                    break;
                case TipoItem::BACULO:
                    equipped = jugador->equiparBaculo(slot);
                    break;
                case TipoItem::ARMADURA:
                    equipped = jugador->equiparArmadura(slot);
                    break;
                case TipoItem::CASCO:
                    equipped = jugador->equiparCasco(slot);
                    break;
                case TipoItem::ESCUDO:
                    equipped = jugador->equiparEscudo(slot);
                    break;
                default:
                    equipped = false;
                    break;
            }

            if (!equipped) {
                snapshots.push_back(
                    Snapshot::error_message(
                        nombre,
                        "No se pudo equipar el item"
                    )
                );
                break;
            }

            snapshots.push_back(Snapshot::player_stats(
                jugador->getNombre(),
                jugador->getRaza()->getNombre(),
                jugador->getClase()->getNombre(),
                static_cast<uint16_t>(jugador->getMapaId()),
                static_cast<uint16_t>(jugador->getPosX()),
                static_cast<uint16_t>(jugador->getPosY()),
                static_cast<uint8_t>(jugador->getDireccion()),
                static_cast<uint16_t>(jugador->getNivel()),
                static_cast<uint16_t>(jugador->getVidaActual()),
                static_cast<uint16_t>(jugador->getVidaMax()),
                static_cast<uint16_t>(jugador->getManaActual()),
                static_cast<uint16_t>(jugador->getManaMax()),
                static_cast<uint32_t>(jugador->getExperiencia()),
                static_cast<uint32_t>(jugador->getOro()),
                static_cast<uint16_t>(jugador->getConstitucion()),
                static_cast<uint16_t>(jugador->getInteligencia()),
                static_cast<uint16_t>(jugador->getFuerza()),
                static_cast<uint16_t>(jugador->getAgilidad())
            ));
            break;
        }

        case protocol::ClientOpcode::BUY_ITEM: {
            snapshots.push_back(
                Snapshot::error_message(
                    nombre,
                    "Compra en comercio todavia no implementada"
                )
            );
            break;
        }

        case protocol::ClientOpcode::SELL_ITEM: {
            snapshots.push_back(
                Snapshot::error_message(
                    nombre,
                    "Venta en comercio todavia no implementada"
                )
            );
            break;
        }

        case protocol::ClientOpcode::DEPOSIT_ITEM: {
            snapshots.push_back(
                Snapshot::error_message(
                    nombre,
                    "Deposito en banco todavia no implementado"
                )
            );
            break;
        }

        case protocol::ClientOpcode::WITHDRAW_ITEM: {
            snapshots.push_back(
                Snapshot::error_message(
                    nombre,
                    "Extraccion de banco todavia no implementada"
                )
            );
            break;
        }

        case protocol::ClientOpcode::PRIVATE_MESSAGE: {
            const std::string destino = cmd.get_nick();
            const std::string mensaje = cmd.get_text();

            if (destino.empty()) {
                snapshots.push_back(
                    Snapshot::error_message(
                        nombre,
                        "Debe indicar un destinatario"
                    )
                );
                break;
            }

            if (mensaje.empty()) {
                snapshots.push_back(
                    Snapshot::error_message(
                        nombre,
                        "El mensaje no puede estar vacio"
                    )
                );
                break;
            }

            if (!getJugador(destino)) {
                snapshots.push_back(
                    Snapshot::error_message(
                        nombre,
                        "El jugador destinatario no existe"
                    )
                );
                break;
            }

            snapshots.push_back(
                Snapshot::chat_message(
                    nombre,
                    destino,
                    mensaje
                )
            );

            break;
        }

        case protocol::ClientOpcode::CLAN_CREATE: {
            snapshots.push_back(
                Snapshot::error_message(
                    nombre,
                    "Creacion de clan todavia no implementada"
                )
            );
            break;
        }

        case protocol::ClientOpcode::CLAN_JOIN: {
            snapshots.push_back(
                Snapshot::error_message(
                    nombre,
                    "Solicitud para unirse a clan todavia no implementada"
                )
            );
            break;
        }

        case protocol::ClientOpcode::CLAN_REVIEW: {
            snapshots.push_back(
                Snapshot::error_message(
                    nombre,
                    "Revision de solicitudes de clan todavia no implementada"
                )
            );
            break;
        }

        case protocol::ClientOpcode::CLAN_ACCEPT: {
            snapshots.push_back(
                Snapshot::error_message(
                    nombre,
                    "Aceptacion de miembro de clan todavia no implementada"
                )
            );
            break;
        }

        case protocol::ClientOpcode::CLAN_REJECT: {
            snapshots.push_back(
                Snapshot::error_message(
                    nombre,
                    "Rechazo de solicitud de clan todavia no implementado"
                )
            );
            break;
        }

        case protocol::ClientOpcode::CLAN_BAN: {
            snapshots.push_back(
                Snapshot::error_message(
                    nombre,
                    "Baneo de miembro de clan todavia no implementado"
                )
            );
            break;
        }

        case protocol::ClientOpcode::CLAN_KICK: {
            snapshots.push_back(
                Snapshot::error_message(
                    nombre,
                    "Expulsion de miembro de clan todavia no implementada"
                )
            );
            break;
        }

        case protocol::ClientOpcode::CLAN_LEAVE: {
            snapshots.push_back(
                Snapshot::error_message(
                    nombre,
                    "Salida de clan todavia no implementada"
                )
            );
            break;
        }

        default: {
            snapshots.push_back(
                Snapshot::error_message(
                    nombre,
                    "Comando no implementado"
                )
            );
            break;
        }
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

    auto itRaza =
        razas.find(razaNombre);
  
    auto itClase =
        clases.find(claseNombre);

    if (itRaza == razas.end() ||
        itClase == clases.end())
        return false;

    auto jugador = std::make_unique<Jugador>(
        nombre, posX, posY,
        itRaza->second.get(),
        itClase->second.get()
    );
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
    return mundo.moverPersonaje(jugador, dir);
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
            mundo.tirarItem(objetivo->getMapaId(), objetivo->getPosX(), objetivo->getPosY(), std::move(item));
        }
    }

    return resultado;
}

bool Game::handle_meditation_interruption(Jugador* jugador, std::vector<Snapshot>& snapshots, const std::string& nombre) {
    if (!jugador || !jugador->estaMeditando()) {
        return false;
    }

    jugador->interrumpirMeditacion();
    snapshots.push_back(Snapshot::meditation_status(nombre, false));
    snapshots.push_back(Snapshot::player_stats(
        jugador->getNombre(),
        jugador->getRaza()->getNombre(),
        jugador->getClase()->getNombre(),
        static_cast<uint16_t>(jugador->getMapaId()),
        static_cast<uint16_t>(jugador->getPosX()),
        static_cast<uint16_t>(jugador->getPosY()),
        static_cast<uint8_t>(jugador->getDireccion()),
        static_cast<uint16_t>(jugador->getNivel()),
        static_cast<uint16_t>(jugador->getVidaActual()),
        static_cast<uint16_t>(jugador->getVidaMax()),
        static_cast<uint16_t>(jugador->getManaActual()),
        static_cast<uint16_t>(jugador->getManaMax()),
        static_cast<uint32_t>(jugador->getExperiencia()),
        static_cast<uint32_t>(jugador->getOro()),
        static_cast<uint16_t>(jugador->getConstitucion()),
        static_cast<uint16_t>(jugador->getInteligencia()),
        static_cast<uint16_t>(jugador->getFuerza()),
        static_cast<uint16_t>(jugador->getAgilidad())
    ));
    return true;
}

std::vector<Snapshot> Game::tick(float dt) {
    std::vector<Snapshot> snapshots;

    for (auto& [nombre, jugador] : jugadores) {
        bool wasMeditating = jugador->estaMeditando();
        int oldMana = jugador->getManaActual();

        jugador->recuperacionPasiva(dt);
        // por ahora mando todo, pero en un futuro podria optimizarse para mandar solo cambios relevantes
        if (wasMeditating && jugador->getManaActual() != oldMana) {
            snapshots.push_back(Snapshot::player_stats(
                jugador->getNombre(),
                jugador->getRaza()->getNombre(),
                jugador->getClase()->getNombre(),
                static_cast<uint16_t>(jugador->getMapaId()),
                static_cast<uint16_t>(jugador->getPosX()),
                static_cast<uint16_t>(jugador->getPosY()),
                static_cast<uint8_t>(jugador->getDireccion()),
                static_cast<uint16_t>(jugador->getNivel()),
                static_cast<uint16_t>(jugador->getVidaActual()),
                static_cast<uint16_t>(jugador->getVidaMax()),
                static_cast<uint16_t>(jugador->getManaActual()),
                static_cast<uint16_t>(jugador->getManaMax()),
                static_cast<uint32_t>(jugador->getExperiencia()),
                static_cast<uint32_t>(jugador->getOro()),
                static_cast<uint16_t>(jugador->getConstitucion()),
                static_cast<uint16_t>(jugador->getInteligencia()),
                static_cast<uint16_t>(jugador->getFuerza()),
                static_cast<uint16_t>(jugador->getAgilidad())
            ));
        }
    }

    // TODO:
    // tick de criaturas
    return snapshots;
}

const Mundo& Game::getMundo() const { return mundo; }

bool Game::tirarItem(const std::string& nombre, int indice, int cantidad) {
    Jugador* jugador = getJugador(nombre);

    if (!jugador || !jugador->estaVivo()) return false;

    auto slot =
        jugador->soltarItem(
            indice,
            cantidad);

    if (!slot)
        return false;

    mundo.tirarItem(jugador->getMapaId(), jugador->getPosX(), jugador->getPosY(), std::move(*slot));
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

    auto slot = mundo.tomarItemEnPosicion(jugador->getMapaId(), jugador->getPosX(), jugador->getPosY(), indice);
    
    if (!slot) return false;
  
    jugador->agarrarItem(std::move(slot->item), slot->cantidad);

    return true;
}

std::vector<PersistenceTask>
Game::build_persistence_tasks_for_command(const Command& cmd) const {
    std::vector<PersistenceTask> tasks;

    const std::string actor =
        getNombreJugadorPorComando(cmd);

    std::vector<std::string> names =
        PersistenceTaskFactory::get_affected_players(cmd, actor);

    for (const std::string& name : names) {
        const Jugador* jugador = getJugador(name);

        if (!jugador) {
            continue;
        }

        tasks.push_back(
            PersistenceTaskFactory::from_player(*jugador));
    }

    return tasks;
}