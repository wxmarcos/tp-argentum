#include "game/game.h"

#include <algorithm>
#include <iostream>

#include "common/protocol_defs.h"
#include "game/formulas.h"
#include "game/items/arma.h"
#include "game/items/inventario.h"
#include "game/items/itemFactory.h"
#include "game/items/item_defs.h"
#include "game/items/oro.h"
#include "game/snapshot_factory.h"
#include "server/persistence/persistence_loader.h"

// ----------------- process() -----------------

std::vector<Snapshot> Game::process(const Command& cmd) {
    std::vector<Snapshot> snapshots;

    // -- LOGIN --------------------------
    if (cmd.get_type() == protocol::ClientOpcode::LOGIN) {
        Jugador* jugador = getJugador(cmd.get_nick());

        if (!jugador) {
            bool restaurado = false;

            auto record = PersistenceLoader::load_player_by_nick(
                config.getRutaJugadores(), config.getRutaIndiceJugadores(),
                cmd.get_nick());

            if (record.has_value()) {
                restaurado = restaurarJugadorPersistido(*record);
            }
            if (!restaurado) {
                snapshots.push_back(Snapshot::error_message(
                    cmd.get_nick(), "Login fallido: personaje inexistente"));
                return snapshots;
            }

            jugador = getJugador(cmd.get_nick());
            if (!jugador) {
                snapshots.push_back(Snapshot::error_message(
                    cmd.get_nick(),
                    "Login fallido: error restaurando personaje"));
                return snapshots;
            }
        }

        player_id_to_nick[cmd.get_player_id()] = cmd.get_nick();

        snapshots.push_back(Snapshot::entity_login(
            cmd.get_nick(), static_cast<uint16_t>(jugador->getMapaId()),
            static_cast<uint16_t>(jugador->getPosX()),
            static_cast<uint16_t>(jugador->getPosY()),
            static_cast<uint8_t>(jugador->getDireccion())));

        snapshots.push_back(Snapshot::map_change(
            cmd.get_nick(), static_cast<uint16_t>(jugador->getMapaId()),
            static_cast<uint16_t>(jugador->getPosX()),
            static_cast<uint16_t>(jugador->getPosY()),
            static_cast<uint8_t>(jugador->getDireccion())));

        snapshots.push_back(SnapshotFactory::player_stats_from_player(*jugador));
        snapshots.push_back(
            SnapshotFactory::player_inventory_from_player(*jugador));

        agregarReplayDeJugadores(snapshots, cmd.get_nick(),
                                 jugador->getMapaId());
        agregarReplayNpcs(snapshots, jugador->getMapaId());
        agregarReplayCriaturas(snapshots, jugador->getMapaId());
        agregarReplayItems(snapshots, jugador->getMapaId());

        // Notificar al clan
        if (jugador->estaEnClan()) {
            for (auto& [nick, j] : jugadores) {
                if (nick == cmd.get_nick()) continue;
                if (j->getClanNombre() == jugador->getClanNombre()) {
                    snapshots.push_back(Snapshot::chat_message(
                        "Sistema", nick,
                        "Tu compañero " + cmd.get_nick() +
                            " entro a Argentum"));
                }
            }
        }

        return snapshots;
    }

    // -- CREATE CHARACTER --------------------------
    if (cmd.get_type() == protocol::ClientOpcode::CREATE_CHARACTER) {
        bool creado = agregarJugador(cmd.get_nick(), config.getSpawnMapaId(),
                                     config.getSpawnX(), config.getSpawnY(),
                                     cmd.get_raza(), cmd.get_clase());

        if (!creado) {
            snapshots.push_back(Snapshot::error_message(
                cmd.get_nick(), "No se pudo crear el personaje"));
            return snapshots;
        }

        Jugador* jugador = getJugador(cmd.get_nick());
        player_id_to_nick[cmd.get_player_id()] = cmd.get_nick();

        // Items de inicio
        jugador->agarrarItem(ItemFactory::crearEspada());
        jugador->agarrarItem(ItemFactory::crearEscudoDeTortuga());

        snapshots.push_back(Snapshot::entity_created(
            cmd.get_nick(), static_cast<uint16_t>(jugador->getMapaId()),
            static_cast<uint16_t>(jugador->getPosX()),
            static_cast<uint16_t>(jugador->getPosY()),
            static_cast<uint8_t>(jugador->getDireccion())));

        snapshots.push_back(Snapshot::map_change(
            cmd.get_nick(), static_cast<uint16_t>(jugador->getMapaId()),
            static_cast<uint16_t>(jugador->getPosX()),
            static_cast<uint16_t>(jugador->getPosY()),
            static_cast<uint8_t>(jugador->getDireccion())));
        snapshots.push_back(SnapshotFactory::player_stats_from_player(*jugador));
        snapshots.push_back(
            SnapshotFactory::player_inventory_from_player(*jugador));

        agregarReplayDeJugadores(snapshots, cmd.get_nick(),
                                 jugador->getMapaId());
        agregarReplayNpcs(snapshots, jugador->getMapaId());
        agregarReplayCriaturas(snapshots, jugador->getMapaId());
        agregarReplayItems(snapshots, jugador->getMapaId());

        return snapshots;
    }

    // -- DISCONNECT --------------------------
    if (cmd.is_disconnect()) {
        const std::string nombre = getNombreJugadorPorComando(cmd);
        if (!nombre.empty()) {
            snapshots.push_back(Snapshot::entity_remove(nombre));

            Jugador* jugadorSaliente = getJugador(nombre);
            if (jugadorSaliente && jugadorSaliente->estaEnClan()) {
                std::string clanNom = jugadorSaliente->getClanNombre();
                for (auto& [nick, j] : jugadores) {
                    if (nick == nombre) continue;
                    if (j->getClanNombre() == clanNom) {
                        snapshots.push_back(Snapshot::chat_message(
                            "Sistema", nick,
                            "Tu compañero " + nombre + " salio de Argentum"));
                    }
                }
            }

            removerJugador(nombre);
            player_id_to_nick.erase(cmd.get_player_id());
        }
        return snapshots;
    }

    // -- Comandos que requieren jugador --------------------------
    const std::string nombre = getNombreJugadorPorComando(cmd);
    if (nombre.empty()) {
        snapshots.push_back(Snapshot::error_message(
            "", "Comando recibido sin jugador asociado"));
        return snapshots;
    }

    Jugador* jugador = getJugador(nombre);

    // Comandos bloqueados para fantasmas
    static const std::initializer_list<protocol::ClientOpcode>
        bloqueadosParaFantasma = {
            protocol::ClientOpcode::ATTACK,
            protocol::ClientOpcode::MEDITATE,
            protocol::ClientOpcode::EQUIP_ITEM,
            protocol::ClientOpcode::PICK_ITEM,
            protocol::ClientOpcode::DROP_ITEM,
            protocol::ClientOpcode::BUY_ITEM,
            protocol::ClientOpcode::SELL_ITEM,
            protocol::ClientOpcode::DEPOSIT_ITEM,
            protocol::ClientOpcode::WITHDRAW_ITEM,
            protocol::ClientOpcode::DEPOSIT_GOLD,
            protocol::ClientOpcode::WITHDRAW_GOLD,
        };
    if (jugador && !jugador->estaVivo()) {
        for (auto op : bloqueadosParaFantasma) {
            if (cmd.get_type() == op) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "No puedes hacer eso siendo un fantasma"));
                return snapshots;
            }
        }
    }

    // Interrumpir meditación
    if (jugador && cmd.get_type() != protocol::ClientOpcode::MEDITATE) {
        handle_meditation_interruption(jugador, snapshots, nombre);
    }

    switch (cmd.get_type()) {
        // ---- Items ----
        case protocol::ClientOpcode::MOVE:
            handleMover(nombre, cmd, snapshots);
            break;
        case protocol::ClientOpcode::PICK_ITEM:
            handlePickItem(nombre, cmd, snapshots);
            break;
        case protocol::ClientOpcode::DROP_ITEM:
            handleDropItem(nombre, cmd, snapshots);
            break;
        case protocol::ClientOpcode::EQUIP_ITEM:
            handleEquipItem(nombre, cmd, snapshots);
            break;

        // ---- Comercio ----
        case protocol::ClientOpcode::BUY_ITEM:
            handleBuyItem(nombre, cmd, snapshots);
            break;
        case protocol::ClientOpcode::SELL_ITEM:
            handleSellItem(nombre, cmd, snapshots);
            break;
        case protocol::ClientOpcode::DEPOSIT_ITEM:
            handleDepositItem(nombre, cmd, snapshots);
            break;
        case protocol::ClientOpcode::WITHDRAW_ITEM:
            handleWithdrawItem(nombre, cmd, snapshots);
            break;
        case protocol::ClientOpcode::DEPOSIT_GOLD:
            handleDepositGold(nombre, cmd, snapshots);
            break;
        case protocol::ClientOpcode::WITHDRAW_GOLD:
            handleWithdrawGold(nombre, cmd, snapshots);
            break;

        // ---- Clanes ----
        case protocol::ClientOpcode::CLAN_CREATE:
            handleClanCreate(nombre, cmd, snapshots);
            break;
        case protocol::ClientOpcode::CLAN_JOIN:
            handleClanJoin(nombre, cmd, snapshots);
            break;
        case protocol::ClientOpcode::CLAN_REVIEW:
            handleClanReview(nombre, snapshots);
            break;
        case protocol::ClientOpcode::CLAN_ACCEPT:
            handleClanAccept(nombre, cmd, snapshots);
            break;
        case protocol::ClientOpcode::CLAN_REJECT:
            handleClanReject(nombre, cmd, snapshots);
            break;
        case protocol::ClientOpcode::CLAN_BAN:
        case protocol::ClientOpcode::CLAN_KICK:
            handleClanBanKick(nombre, cmd, snapshots);
            break;
        case protocol::ClientOpcode::CLAN_LEAVE:
            handleClanLeave(nombre, snapshots);
            break;

        // ---- Combate ----
        case protocol::ClientOpcode::ATTACK: {
            const std::string objetivo = cmd.get_nick();
            ResultadoAtaque resultado = atacar(nombre, objetivo);

            if (!resultado.exito) {
                snapshots.push_back(
                    Snapshot::error_message(nombre, "Ataque invalido"));
                break;
            }
            if (resultado.fueEsquivado) {
                snapshots.push_back(Snapshot::dodge_event(nombre, objetivo));
                snapshots.push_back(Snapshot::chat_message(
                    "Sistema", nombre, objetivo + " esquivo tu ataque"));
                snapshots.push_back(Snapshot::chat_message(
                    "Sistema", objetivo, "Esquivaste el ataque de " + nombre));
                break;
            }

            snapshots.push_back(Snapshot::damage_event(
                nombre, objetivo,
                static_cast<uint16_t>(resultado.danioAplicado),
                resultado.fueCritico));

            {
                std::string sufijo = resultado.fueCritico ? " (CRITICO!)" : "";
                snapshots.push_back(Snapshot::chat_message(
                    "Sistema", nombre,
                    "Le hiciste " + std::to_string(resultado.danioAplicado) +
                        " de daño a " + objetivo + sufijo));
                snapshots.push_back(Snapshot::chat_message(
                    "Sistema", objetivo,
                    "Recibiste " + std::to_string(resultado.danioAplicado) +
                        " de daño de " + nombre + sufijo));

                // Notificar al clan de la victima
                if (Jugador* victima = getJugador(objetivo)) {
                    if (victima->estaEnClan()) {
                        for (auto& [nick, j] : jugadores) {
                            if (nick == objetivo) continue;
                            if (j->getClanNombre() ==
                                victima->getClanNombre()) {
                                snapshots.push_back(Snapshot::chat_message(
                                    "Sistema", nick,
                                    "Tu compañero " + objetivo +
                                        " esta siendo atacado por " + nombre));
                            }
                        }
                    }
                }
            }

            snapshots.push_back(
                SnapshotFactory::player_stats_from_player(*jugador));
            if (Jugador* victima = getJugador(objetivo)) {
                snapshots.push_back(
                    SnapshotFactory::player_stats_from_player(*victima));
            }

            if (resultado.objetivoMurio) {
                snapshots.push_back(Snapshot::death_event(objetivo));
                snapshots.push_back(Snapshot::entity_remove(objetivo));
                snapshots.push_back(Snapshot::chat_message(
                    "Sistema", nombre, "Mataste a " + objetivo));

                if (Criatura* criatura = getCriatura(objetivo)) {
                    procesarDropCriatura(objetivo, jugador, criatura,
                                         snapshots);
                    removerCriatura(objetivo);
                } else if (Jugador* victima = getJugador(objetivo)) {
                    auto items = victima->soltarTodosLosItems();
                    for (auto& item : items) {
                        std::string nombreItem = item.item->getNombre();
                        uint16_t cantidad = item.cantidad;
                        mundo.tirarItem(victima->getMapaId(),
                                        victima->getPosX(), victima->getPosY(),
                                        std::move(item));
                        snapshots.push_back(Snapshot::item_event(
                            static_cast<uint8_t>(
                                protocol::ItemEventAction::DROP),
                            victima->getNombre(), nombreItem,
                            static_cast<uint16_t>(victima->getMapaId()),
                            static_cast<uint16_t>(victima->getPosX()),
                            static_cast<uint16_t>(victima->getPosY()),
                            cantidad));
                    }

                    int oroExceso = Formulas::calcularOroExceso(
                        victima->getOro(), victima->getOroMax());
                    if (oroExceso > 0) {
                        victima->gastarOro(oroExceso);
                        mundo.tirarItem(
                            victima->getMapaId(), victima->getPosX(),
                            victima->getPosY(),
                            SlotInventario(ItemFactory::crearOro(oroExceso)));
                        snapshots.push_back(Snapshot::item_event(
                            static_cast<uint8_t>(
                                protocol::ItemEventAction::DROP),
                            victima->getNombre(), item_defs::ORO,
                            static_cast<uint16_t>(victima->getMapaId()),
                            static_cast<uint16_t>(victima->getPosX()),
                            static_cast<uint16_t>(victima->getPosY()),
                            static_cast<uint16_t>(oroExceso)));
                    }

                    snapshots.push_back(
                        SnapshotFactory::player_stats_from_player(*jugador));
                    snapshots.push_back(
                        SnapshotFactory::player_stats_from_player(*victima));
                    snapshots.push_back(
                        SnapshotFactory::player_inventory_from_player(*victima));
                }
            }
            break;
        }

        // ---- Meditación ----
        case protocol::ClientOpcode::MEDITATE: {
            if (!jugador || !jugador->estaVivo()) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "No se puede meditar si no estas vivo"));
                break;
            }
            if (!jugador->getClase()->puedeMeditar()) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Tu clase no puede meditar"));
                break;
            }
            if (jugador->estaMeditando()) break;
            jugador->iniciarMeditacion();
            snapshots.push_back(Snapshot::meditation_status(nombre, true));
            snapshots.push_back(
                SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }

        // ---- Cheats ----
        case protocol::ClientOpcode::CHEAT_GOD: {
            if (!jugador) break;
            bool activo = jugador->toggleCheatVidaInfinita();
            snapshots.push_back(Snapshot::cheat_status(
                nombre,
                static_cast<uint8_t>(protocol::ClientOpcode::CHEAT_GOD),
                activo));
            snapshots.push_back(
                SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }
        case protocol::ClientOpcode::CHEAT_MANA: {
            if (!jugador) break;
            bool activo = jugador->toggleCheatManaInfinito();
            snapshots.push_back(Snapshot::cheat_status(
                nombre,
                static_cast<uint8_t>(protocol::ClientOpcode::CHEAT_MANA),
                activo));
            snapshots.push_back(
                SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }
        case protocol::ClientOpcode::CHEAT_DIE: {
            if (!jugador) break;
            jugador->morir();
            snapshots.push_back(Snapshot::cheat_status(
                nombre,
                static_cast<uint8_t>(protocol::ClientOpcode::CHEAT_DIE),
                !jugador->estaVivo()));
            if (!jugador->estaVivo()) {
                snapshots.push_back(Snapshot::death_event(nombre));
                snapshots.push_back(Snapshot::entity_remove(nombre));
                auto items = jugador->soltarTodosLosItems();
                for (auto& item : items) {
                    std::string nombreItem = item.item->getNombre();
                    uint16_t cantidad = item.cantidad;
                    mundo.tirarItem(jugador->getMapaId(), jugador->getPosX(),
                                    jugador->getPosY(), std::move(item));
                    snapshots.push_back(Snapshot::item_event(
                        static_cast<uint8_t>(protocol::ItemEventAction::DROP),
                        jugador->getNombre(), nombreItem,
                        static_cast<uint16_t>(jugador->getMapaId()),
                        static_cast<uint16_t>(jugador->getPosX()),
                        static_cast<uint16_t>(jugador->getPosY()), cantidad));
                }
                int oroExceso = Formulas::calcularOroExceso(jugador->getOro(),
                                                            jugador->getOroMax());
                if (oroExceso > 0) {
                    jugador->gastarOro(oroExceso);
                    mundo.tirarItem(
                        jugador->getMapaId(), jugador->getPosX(),
                        jugador->getPosY(),
                        SlotInventario(ItemFactory::crearOro(oroExceso)));
                    snapshots.push_back(Snapshot::item_event(
                        static_cast<uint8_t>(protocol::ItemEventAction::DROP),
                        jugador->getNombre(), item_defs::ORO,
                        static_cast<uint16_t>(jugador->getMapaId()),
                        static_cast<uint16_t>(jugador->getPosX()),
                        static_cast<uint16_t>(jugador->getPosY()),
                        static_cast<uint16_t>(oroExceso)));
                }
            }
            snapshots.push_back(
                SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }
        case protocol::ClientOpcode::CHEAT_RESURRECT: {
            if (!jugador) break;
            jugador->revivir(jugador->getVidaMax());
            snapshots.push_back(Snapshot::cheat_status(
                nombre,
                static_cast<uint8_t>(protocol::ClientOpcode::CHEAT_RESURRECT),
                jugador->estaVivo()));
            snapshots.push_back(
                SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }

        // ---- Resurrección ----
        case protocol::ClientOpcode::RESURRECT: {
            if (!jugador || jugador->estaVivo()) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Solo un fantasma puede usar /resucitar"));
                break;
            }
            if (jugador->estaResucitando()) {
                snapshots.push_back(
                    Snapshot::error_message(nombre, "Ya estas resucitando"));
                break;
            }
            InfoNPC destino;
            float distancia;
            if (!encontrarSacerdoteMasCercano(jugador, destino, distancia)) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "No hay ningun sacerdote en el mundo"));
                break;
            }
            float tiempo = distancia / config.getVelocidadResurreccion();
            jugador->iniciarResurreccion(tiempo, destino.mapaId, destino.x,
                                         destino.y);
            break;
        }

        // ---- Curación ----
        case protocol::ClientOpcode::HEAL: {
            if (!jugador || !jugador->estaVivo()) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "No puedes curarte si eres un fantasma"));
                break;
            }
            if (!hayNPCCercano(jugador, sacerdotes)) {
                snapshots.push_back(Snapshot::error_message(
                    nombre,
                    "Debes estar cerca de un sacerdote para curarte"));
                break;
            }
            jugador->curar(jugador->getVidaMax());
            jugador->recuperarMana(jugador->getManaMax());
            snapshots.push_back(
                SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }

        // ---- Chat privado ----
        case protocol::ClientOpcode::PRIVATE_MESSAGE: {
            const std::string destino = cmd.get_nick();
            const std::string mensaje = cmd.get_text();
            if (destino.empty()) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Debe indicar un destinatario"));
                break;
            }
            if (mensaje.empty()) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "El mensaje no puede estar vacio"));
                break;
            }
            if (!getJugador(destino)) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "El jugador destinatario no existe"));
                break;
            }
            snapshots.push_back(
                Snapshot::chat_message(nombre, destino, mensaje));
            break;
        }

        default:
            snapshots.push_back(
                Snapshot::error_message(nombre, "Comando no implementado"));
            break;
    }

    return snapshots;
}

// ----------------- NPC helpers -----------------

bool Game::hayNPCCercano(const Jugador* jugador,
                         const std::vector<InfoNPC>& npcs) const {
    for (const auto& npc : npcs) {
        if (npc.mapaId != jugador->getMapaId()) continue;
        int dx = std::abs(npc.x - jugador->getPosX());
        int dy = std::abs(npc.y - jugador->getPosY());
        if (dx + dy <= 10) return true;
    }
    return false;
}
