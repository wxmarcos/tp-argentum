#include <algorithm>
#include <iostream>
#include <set>

#include "common/protocol_defs.h"
#include "game/formulas.h"
#include "game/game.h"
#include "game/items/arma.h"
#include "game/items/inventario.h"
#include "game/items/itemFactory.h"
#include "game/items/item_defs.h"
#include "game/items/oro.h"
#include "game/snapshot_factory.h"
#include "server/persistence/players/persistence_loader.h"

// ----------------- process() -----------------

std::vector<OutgoingSnapshot> Game::process(const Command& cmd) {
    std::vector<OutgoingSnapshot> snapshots;
    uint16_t playerId = cmd.get_player_id();
    // -- LOGIN --------------------------
    if (cmd.get_type() == protocol::ClientOpcode::LOGIN) {
        Jugador* jugador = getJugador(cmd.get_nick());
        if (nick_to_player_id.find(cmd.get_nick()) != nick_to_player_id.end()) {
            push_unicast(snapshots,
                         Snapshot::error_message(
                             cmd.get_nick(),
                             "Login fallido: la cuenta ya esta conectada"),
                         playerId);
            return snapshots;
        }
        if (!jugador) {
            bool restaurado = false;

            auto record = PersistenceLoader::load_player_by_nick(
                config.getRutaJugadores(), config.getRutaIndiceJugadores(),
                cmd.get_nick());

            if (record.has_value()) {
                restaurado = restaurarJugadorPersistido(*record);
            }
            if (!restaurado) {
                push_unicast(
                    snapshots,
                    Snapshot::error_message(
                        cmd.get_nick(), "Login fallido: personaje inexistente"),
                    playerId);
                return snapshots;
            }

            jugador = getJugador(cmd.get_nick());
            if (!jugador) {
                push_unicast(snapshots,
                             Snapshot::error_message(
                                 cmd.get_nick(),
                                 "Login fallido: error restaurando personaje"),
                             playerId);
                return snapshots;
            }
        }

        player_id_to_nick[cmd.get_player_id()] = cmd.get_nick();
        nick_to_player_id[cmd.get_nick()] = playerId;

        push_broadcast(
            snapshots,
            Snapshot::entity_login(
                cmd.get_nick(), static_cast<uint16_t>(jugador->getMapaId()),
                static_cast<uint16_t>(jugador->getPosX()),
                static_cast<uint16_t>(jugador->getPosY()),
                static_cast<uint8_t>(jugador->getDireccion())));

        push_broadcast(snapshots,
                       SnapshotFactory::player_stats_from_player(*jugador));

        push_broadcast(
            snapshots,
            Snapshot::map_change(
                cmd.get_nick(), static_cast<uint16_t>(jugador->getMapaId()),
                static_cast<uint16_t>(jugador->getPosX()),
                static_cast<uint16_t>(jugador->getPosY()),
                static_cast<uint8_t>(jugador->getDireccion())));

        push_broadcast(snapshots,
                       SnapshotFactory::player_inventory_from_player(*jugador));

        replay(snapshots, cmd, playerId);
        // Notificar al clan
        if (jugador->estaEnClan()) {
            for (auto& [nick, j] : jugadores) {
                if (nick == cmd.get_nick()) continue;
                if (j->getClanNombre() == jugador->getClanNombre()) {
                    auto itId = nick_to_player_id.find(nick);
                    if (itId != nick_to_player_id.end()) {
                        push_unicast(snapshots,
                                     Snapshot::chat_message(
                                         "Sistema", nick,
                                         "Tu compañero " + cmd.get_nick() +
                                             " entró a Argentum"),
                                     itId->second);
                    }
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
            push_unicast(snapshots,
                         Snapshot::error_message(
                             cmd.get_nick(), "No se pudo crear el personaje"),
                         playerId);
            return snapshots;
        }

        Jugador* jugador = getJugador(cmd.get_nick());
        player_id_to_nick[playerId] = cmd.get_nick();
        nick_to_player_id[cmd.get_nick()] = playerId;

        jugador->agarrarItem(ItemFactory::crear(item_defs::ESPADA, config));

        push_unicast(
            snapshots,
            Snapshot::map_change(cmd.get_nick(),
                                 static_cast<uint16_t>(jugador->getMapaId()),
                                 static_cast<uint16_t>(jugador->getPosX()),
                                 static_cast<uint16_t>(jugador->getPosY()),
                                 static_cast<uint8_t>(jugador->getDireccion())),
            playerId);

        push_unicast(snapshots,
                     SnapshotFactory::player_stats_from_player(*jugador),
                     playerId);

        push_unicast(snapshots,
                     SnapshotFactory::player_inventory_from_player(*jugador),
                     playerId);

        push_broadcast(
            snapshots,
            Snapshot::entity_created(
                cmd.get_nick(), static_cast<uint16_t>(jugador->getMapaId()),
                static_cast<uint16_t>(jugador->getPosX()),
                static_cast<uint16_t>(jugador->getPosY()),
                static_cast<uint8_t>(jugador->getDireccion())));
        replay(snapshots, cmd, playerId);
        return snapshots;
    }

    // -- DISCONNECT --------------------------
    if (cmd.is_disconnect()) {
        const std::string nombre = getNombreJugadorPorComando(cmd);
        if (!nombre.empty()) {
            push_broadcast(snapshots, Snapshot::entity_remove(nombre));

            Jugador* jugadorSaliente = getJugador(nombre);
            if (jugadorSaliente && jugadorSaliente->estaEnClan()) {
                std::string clanNom = jugadorSaliente->getClanNombre();
                for (auto& [nick, j] : jugadores) {
                    if (nick == nombre) continue;
                    if (j->getClanNombre() == clanNom) {
                        auto itId = nick_to_player_id.find(nick);
                        if (itId != nick_to_player_id.end()) {
                            push_unicast(snapshots,
                                         Snapshot::chat_message(
                                             "Sistema", nick,
                                             "Tu compañero " + nombre +
                                                 " salió de Argentum"),
                                         itId->second);
                        }
                    }
                }
            }

            removerJugador(nombre);
            player_id_to_nick.erase(cmd.get_player_id());
            nick_to_player_id.erase(nombre);
        }
        return snapshots;
    }

    // -- Comandos que requieren jugador --------------------------
    const std::string nombre = getNombreJugadorPorComando(cmd);
    if (nombre.empty()) {
        push_unicast(snapshots,
                     Snapshot::error_message(
                         "", "Comando recibido sin jugador asociado"),
                     playerId);
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
            protocol::ClientOpcode::LIST_ITEMS,
        };
    if (jugador && !jugador->estaVivo()) {
        for (auto op : bloqueadosParaFantasma) {
            if (cmd.get_type() == op) {
                push_unicast(
                    snapshots,
                    Snapshot::error_message(
                        nombre, "No puedes hacer eso siendo un fantasma"),
                    playerId);
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
            handleMover(nombre, cmd, snapshots, playerId);
            break;
        case protocol::ClientOpcode::PICK_ITEM:
            handlePickItem(nombre, cmd, snapshots, playerId);
            break;
        case protocol::ClientOpcode::DROP_ITEM:
            handleDropItem(nombre, cmd, snapshots, playerId);
            break;
        case protocol::ClientOpcode::EQUIP_ITEM:
            handleEquipItem(nombre, cmd, snapshots, playerId);
            break;

        // ---- Comercio ----
        case protocol::ClientOpcode::BUY_ITEM:
            handleBuyItem(nombre, cmd, snapshots, playerId);
            break;
        case protocol::ClientOpcode::SELL_ITEM:
            handleSellItem(nombre, cmd, snapshots, playerId);
            break;
        case protocol::ClientOpcode::DEPOSIT_ITEM:
            handleDepositItem(nombre, cmd, snapshots, playerId);
            break;
        case protocol::ClientOpcode::WITHDRAW_ITEM:
            handleWithdrawItem(nombre, cmd, snapshots, playerId);
            break;
        case protocol::ClientOpcode::DEPOSIT_GOLD:
            handleDepositGold(nombre, cmd, snapshots, playerId);
            break;
        case protocol::ClientOpcode::WITHDRAW_GOLD:
            handleWithdrawGold(nombre, cmd, snapshots, playerId);
            break;
        case protocol::ClientOpcode::LIST_ITEMS:
            handleListItems(nombre, snapshots, playerId);
            break;

        // ---- Clanes ----
        case protocol::ClientOpcode::CLAN_CREATE:
            handleClanCreate(nombre, cmd, snapshots, playerId);
            break;
        case protocol::ClientOpcode::CLAN_JOIN:
            handleClanJoin(nombre, cmd, snapshots, playerId);
            break;
        case protocol::ClientOpcode::CLAN_REVIEW:
            handleClanReview(nombre, snapshots, playerId);
            break;
        case protocol::ClientOpcode::CLAN_ACCEPT:
            handleClanAccept(nombre, cmd, snapshots, playerId);
            break;
        case protocol::ClientOpcode::CLAN_REJECT:
            handleClanReject(nombre, cmd, snapshots, playerId);
            break;
        case protocol::ClientOpcode::CLAN_BAN:
        case protocol::ClientOpcode::CLAN_KICK:
            handleClanBanKick(nombre, cmd, snapshots, playerId);
            break;
        case protocol::ClientOpcode::CLAN_LEAVE:
            handleClanLeave(nombre, snapshots, playerId);
            break;

        // ---- Combate ----
        case protocol::ClientOpcode::ATTACK: {
            const std::string objetivo = cmd.get_nick();
            ResultadoAtaque resultado = atacar(nombre, objetivo);

            if (!resultado.exito) {
                const std::string msg =
                    resultado.fueraDeRango ? "Estas demasiado lejos para atacar"
                                           : "Ataque invalido";

                push_unicast(snapshots, Snapshot::error_message(nombre, msg),
                             playerId);

                break;
            }
            if (resultado.fueEsquivado) {
                push_broadcast(snapshots,
                               Snapshot::dodge_event(nombre, objetivo));
                break;
            }

            push_broadcast(snapshots,
                           Snapshot::damage_event(
                               nombre, objetivo,
                               static_cast<uint16_t>(resultado.danioAplicado),
                               resultado.fueCritico));

            {
                // Notificar al clan de la victima
                if (Jugador* victima = getJugador(objetivo)) {
                    if (victima->estaEnClan()) {
                        for (auto& [nick, j] : jugadores) {
                            if (nick == objetivo) continue;
                            if (j->getClanNombre() ==
                                victima->getClanNombre()) {
                                auto itId = nick_to_player_id.find(nick);
                                if (itId != nick_to_player_id.end()) {
                                    push_unicast(
                                        snapshots,
                                        Snapshot::chat_message(
                                            "Sistema", nick,
                                            "Tu compañero " + objetivo +
                                                " esta siendo atacado por " +
                                                nombre),
                                        itId->second);
                                }
                            }
                        }
                    }
                }
            }

            push_broadcast(snapshots,
                           SnapshotFactory::player_stats_from_player(*jugador));
            if (Jugador* victima = getJugador(objetivo)) {
                push_broadcast(
                    snapshots,
                    SnapshotFactory::player_stats_from_player(*victima));
            }

            if (resultado.objetivoMurio) {
                push_broadcast(snapshots, Snapshot::death_event(objetivo));
                push_broadcast(snapshots, Snapshot::entity_remove(objetivo));

                if (Criatura* criatura = getCriatura(objetivo)) {
                    procesarDropCriatura(objetivo, jugador, criatura,
                                         snapshots);
                    removerCriatura(objetivo);
                } else if (Jugador* victima = getJugador(objetivo)) {
                    auto items = victima->soltarTodosLosItems();
                    std::set<std::pair<int, int>> tilesUsados;
                    for (auto& item : items) {
                        std::string nombreItem = item.item->getNombre();
                        uint16_t cantidad = item.cantidad;
                        auto [tx, ty] = buscarTileParaItem(
                            victima->getMapaId(), victima->getPosX(),
                            victima->getPosY(), tilesUsados);
                        mundo.tirarItem(victima->getMapaId(), tx, ty,
                                        std::move(item));
                        push_broadcast(
                            snapshots,
                            Snapshot::item_event(
                                static_cast<uint8_t>(
                                    protocol::ItemEventAction::DROP),
                                victima->getNombre(), nombreItem,
                                static_cast<uint16_t>(victima->getMapaId()),
                                static_cast<uint16_t>(tx),
                                static_cast<uint16_t>(ty), cantidad));
                    }

                    int oroExceso = Formulas::calcularOroExceso(
                        victima->getOro(), victima->getOroMax());
                    if (oroExceso > 0) {
                        victima->gastarOro(oroExceso);
                        auto [tx, ty] = buscarTileParaItem(
                            victima->getMapaId(), victima->getPosX(),
                            victima->getPosY(), tilesUsados);
                        mundo.tirarItem(
                            victima->getMapaId(), tx, ty,
                            SlotInventario(ItemFactory::crearOro(oroExceso)));
                        push_broadcast(
                            snapshots,
                            Snapshot::item_event(
                                static_cast<uint8_t>(
                                    protocol::ItemEventAction::DROP),
                                victima->getNombre(), item_defs::ORO,
                                static_cast<uint16_t>(victima->getMapaId()),
                                static_cast<uint16_t>(tx),
                                static_cast<uint16_t>(ty),
                                static_cast<uint16_t>(oroExceso)));
                    }

                    push_broadcast(
                        snapshots,
                        SnapshotFactory::player_stats_from_player(*jugador));
                    push_broadcast(
                        snapshots,
                        SnapshotFactory::player_stats_from_player(*victima));
                    push_broadcast(
                        snapshots,
                        SnapshotFactory::player_inventory_from_player(
                            *victima));
                }
            }
            break;
        }

        // ---- Meditación ----
        case protocol::ClientOpcode::MEDITATE: {
            if (!jugador || !jugador->estaVivo()) {
                push_unicast(
                    snapshots,
                    Snapshot::error_message(
                        nombre, "No se puede meditar si no estas vivo"),
                    playerId);
                break;
            }
            if (!jugador->getClase()->puedeMeditar()) {
                push_unicast(snapshots,
                             Snapshot::error_message(
                                 nombre, "Tu clase no puede meditar"),
                             playerId);
                break;
            }
            if (jugador->estaMeditando()) break;
            jugador->iniciarMeditacion();
            push_broadcast(snapshots,
                           Snapshot::meditation_status(nombre, true));
            push_broadcast(snapshots,
                           SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }

        // ---- Cheats ----
        case protocol::ClientOpcode::CHEAT_GOD: {
            if (!jugador) break;
            bool activo = jugador->toggleCheatVidaInfinita();
            push_broadcast(
                snapshots,
                Snapshot::cheat_status(
                    nombre,
                    static_cast<uint8_t>(protocol::ClientOpcode::CHEAT_GOD),
                    activo));
            push_broadcast(snapshots,
                           SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }
        case protocol::ClientOpcode::CHEAT_MANA: {
            if (!jugador) break;
            bool activo = jugador->toggleCheatManaInfinito();
            push_broadcast(
                snapshots,
                Snapshot::cheat_status(
                    nombre,
                    static_cast<uint8_t>(protocol::ClientOpcode::CHEAT_MANA),
                    activo));
            push_broadcast(snapshots,
                           SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }
        case protocol::ClientOpcode::CHEAT_DIE: {
            if (!jugador) break;
            jugador->morir();
            push_broadcast(
                snapshots,
                Snapshot::cheat_status(
                    nombre,
                    static_cast<uint8_t>(protocol::ClientOpcode::CHEAT_DIE),
                    !jugador->estaVivo()));
            if (!jugador->estaVivo()) {
                push_broadcast(snapshots, Snapshot::death_event(nombre));
                push_broadcast(snapshots, Snapshot::entity_remove(nombre));
                auto items = jugador->soltarTodosLosItems();
                std::set<std::pair<int, int>> tilesUsados;
                for (auto& item : items) {
                    std::string nombreItem = item.item->getNombre();
                    uint16_t cantidad = item.cantidad;
                    auto [tx, ty] = buscarTileParaItem(
                        jugador->getMapaId(), jugador->getPosX(),
                        jugador->getPosY(), tilesUsados);
                    mundo.tirarItem(jugador->getMapaId(), tx, ty,
                                    std::move(item));
                    push_broadcast(
                        snapshots,
                        Snapshot::item_event(
                            static_cast<uint8_t>(
                                protocol::ItemEventAction::DROP),
                            jugador->getNombre(), nombreItem,
                            static_cast<uint16_t>(jugador->getMapaId()),
                            static_cast<uint16_t>(tx),
                            static_cast<uint16_t>(ty), cantidad));
                }
                int oroExceso = Formulas::calcularOroExceso(
                    jugador->getOro(), jugador->getOroMax());
                if (oroExceso > 0) {
                    jugador->gastarOro(oroExceso);
                    auto [tx, ty] = buscarTileParaItem(
                        jugador->getMapaId(), jugador->getPosX(),
                        jugador->getPosY(), tilesUsados);
                    mundo.tirarItem(
                        jugador->getMapaId(), tx, ty,
                        SlotInventario(ItemFactory::crearOro(oroExceso)));
                    push_broadcast(
                        snapshots,
                        Snapshot::item_event(
                            static_cast<uint8_t>(
                                protocol::ItemEventAction::DROP),
                            jugador->getNombre(), item_defs::ORO,
                            static_cast<uint16_t>(jugador->getMapaId()),
                            static_cast<uint16_t>(tx),
                            static_cast<uint16_t>(ty),
                            static_cast<uint16_t>(oroExceso)));
                }
            }
            push_broadcast(snapshots,
                           SnapshotFactory::player_stats_from_player(*jugador));
            push_unicast(
                snapshots,
                SnapshotFactory::player_inventory_from_player(*jugador),
                playerId);
            break;
        }
        case protocol::ClientOpcode::CHEAT_RESURRECT: {
            if (!jugador) break;
            jugador->revivir(jugador->getVidaMax());
            push_broadcast(snapshots,
                           Snapshot::cheat_status(
                               nombre,
                               static_cast<uint8_t>(
                                   protocol::ClientOpcode::CHEAT_RESURRECT),
                               jugador->estaVivo()));
            push_broadcast(snapshots,
                           SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }

        // ---- Resurrección ----
        case protocol::ClientOpcode::RESURRECT: {
            if (!jugador || jugador->estaVivo()) {
                push_unicast(
                    snapshots,
                    Snapshot::error_message(
                        nombre, "Solo un fantasma puede usar /resucitar"),
                    playerId);
                break;
            }
            if (jugador->estaResucitando()) {
                push_unicast(
                    snapshots,
                    Snapshot::error_message(nombre, "Ya estas resucitando"),
                    playerId);
                break;
            }
            InfoNPC destino;
            float distancia;
            if (!encontrarSacerdoteMasCercano(jugador, destino, distancia)) {
                push_unicast(snapshots,
                             Snapshot::error_message(
                                 nombre, "No hay ningun sacerdote en el mundo"),
                             playerId);
                break;
            }
            float tiempo = distancia / config.getVelocidadResurreccion();
            int resX = destino.x;
            int resY = destino.y;

            if (!buscarPosicionLibreCerca(destino.mapaId, destino.x, destino.y,
                                          resX, resY)) {
                push_unicast(
                    snapshots,
                    Snapshot::error_message(
                        nombre, "No hay espacio libre cerca del sacerdote"),
                    playerId);
                break;
            }

            jugador->iniciarResurreccion(tiempo, destino.mapaId, resX, resY);
            push_unicast(
                snapshots,
                Snapshot::chat_message(
                    "Sistema", nombre,
                    "Resucitando en " +
                        std::to_string(static_cast<int>(tiempo)) + " segundos"),
                playerId);
            break;
        }

        // ---- Curación ----
        case protocol::ClientOpcode::HEAL: {
            if (!jugador || !jugador->estaVivo()) {
                push_unicast(
                    snapshots,
                    Snapshot::error_message(
                        nombre, "No puedes curarte si eres un fantasma"),
                    playerId);
                break;
            }
            if (!hayNPCCercano(jugador, sacerdotes)) {
                push_broadcast(
                    snapshots,
                    Snapshot::error_message(
                        nombre,
                        "Debes estar cerca de un sacerdote para curarte"));
                break;
            }
            jugador->curar(jugador->getVidaMax());
            jugador->recuperarMana(jugador->getManaMax());
            push_broadcast(snapshots,
                           SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }

        // ---- Chat privado ----
        case protocol::ClientOpcode::PRIVATE_MESSAGE: {
            const std::string destino = cmd.get_nick();
            const std::string mensaje = cmd.get_text();

            if (destino.empty()) {
                push_unicast(snapshots,
                             Snapshot::error_message(
                                 nombre, "Debe indicar un destinatario"),
                             playerId);
                break;
            }

            if (mensaje.empty()) {
                push_unicast(snapshots,
                             Snapshot::error_message(
                                 nombre, "El mensaje no puede estar vacio"),
                             playerId);
                break;
            }

            if (!getJugador(destino)) {
                push_unicast(snapshots,
                             Snapshot::error_message(
                                 nombre, "El jugador destinatario no existe"),
                             playerId);
                break;
            }

            auto itDestino = nick_to_player_id.find(destino);

            if (itDestino == nick_to_player_id.end()) {
                push_unicast(
                    snapshots,
                    Snapshot::error_message(
                        nombre, "El jugador destinatario no esta conectado"),
                    playerId);
                break;
            }

            std::vector<uint16_t> recipients = {playerId};

            if (itDestino->second != playerId) {
                recipients.push_back(itDestino->second);
            }

            push_multicast(snapshots,
                           Snapshot::chat_message(nombre, destino, mensaje),
                           std::move(recipients));

            break;
        }
        default:
            push_unicast(
                snapshots,
                Snapshot::error_message(nombre, "Comando no implementado"),
                playerId);
            break;
    }

    return snapshots;
}

// ----------------- MOVE -----------------

void Game::handleMover(const std::string& nombre, const Command& cmd,
                       std::vector<OutgoingSnapshot>& snapshots,
                       uint16_t playerId) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador) {
        push_unicast(snapshots,
                     Snapshot::error_message(nombre, "Jugador inexistente"),
                     playerId);
        return;
    }

    if (!puedeMoverAhora(nombre)) return;

    int mapaAnterior = jugador->getMapaId();

    bool moved =
        moverJugador(nombre, static_cast<Direccion>(cmd.get_direction()));

    if (!moved) {
        push_unicast(snapshots,
                     Snapshot::error_message(nombre, "Movimiento invalido"),
                     playerId);

        push_broadcast(snapshots,
                       Snapshot::entity_move(
                           nombre, static_cast<uint16_t>(jugador->getMapaId()),
                           static_cast<uint16_t>(jugador->getPosX()),
                           static_cast<uint16_t>(jugador->getPosY()),
                           static_cast<uint8_t>(jugador->getDireccion())));
        return;
    }

    int mapaActual = jugador->getMapaId();

    if (mapaActual != mapaAnterior) {
        // Que todos borren al jugador de su mapa viejo.
        push_broadcast(snapshots, Snapshot::entity_remove(nombre));

        // Al jugador que cambió de mapa: actualizar mapa/posición local.
        push_unicast(
            snapshots,
            Snapshot::map_change(nombre, static_cast<uint16_t>(mapaActual),
                                 static_cast<uint16_t>(jugador->getPosX()),
                                 static_cast<uint16_t>(jugador->getPosY()),
                                 static_cast<uint8_t>(jugador->getDireccion())),
            playerId);

        // Al jugador que cambió de mapa: asegurar stats/skin local.
        push_unicast(snapshots,
                     SnapshotFactory::player_stats_from_player(*jugador),
                     playerId);
        // Al jugador que cambió de mapa: asegurar items local.
        push_broadcast(snapshots,
                       SnapshotFactory::player_inventory_from_player(*jugador));
        // A los demás: aparece el jugador en el mapa nuevo.
        push_broadcast(snapshots,
                       Snapshot::entity_created(
                           nombre, static_cast<uint16_t>(mapaActual),
                           static_cast<uint16_t>(jugador->getPosX()),
                           static_cast<uint16_t>(jugador->getPosY()),
                           static_cast<uint8_t>(jugador->getDireccion())));

        // A los demás: raza/clase para dibujarlo con skin correcta.
        push_broadcast(snapshots,
                       SnapshotFactory::player_stats_from_player(*jugador));

        // Al jugador que entra al mapa nuevo:
        // recibir jugadores, NPCs, criaturas e items ya existentes.
        agregarReplayDeJugadores(snapshots, nombre, mapaActual, playerId);
        agregarReplayNpcs(snapshots, mapaActual, playerId);
        agregarReplayCriaturas(snapshots, mapaActual, playerId);
        agregarReplayItems(snapshots, mapaActual, playerId);

    } else {
        push_broadcast(snapshots,
                       Snapshot::entity_move(
                           nombre, static_cast<uint16_t>(mapaActual),
                           static_cast<uint16_t>(jugador->getPosX()),
                           static_cast<uint16_t>(jugador->getPosY()),
                           static_cast<uint8_t>(jugador->getDireccion())));
    }
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

// ----------------- Replay Helper -----------------
void Game::replay(std::vector<OutgoingSnapshot>& snapshots, const Command& cmd,
                  uint16_t playerId) {
    const std::string& nick = cmd.get_nick();
    Jugador* jugador = getJugador(nick);
    if (!jugador) return;

    agregarReplayDeJugadores(snapshots, nick, jugador->getMapaId(), playerId);
    agregarReplayNpcs(snapshots, jugador->getMapaId(), playerId);
    agregarReplayCriaturas(snapshots, jugador->getMapaId(), playerId);
    agregarReplayItems(snapshots, jugador->getMapaId(), playerId);
}
