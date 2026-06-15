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

// TODO : mandar snapshots que tengan sentido con cada accion
std::vector<OutgoingSnapshot> Game::process(const Command& cmd) {
    std::vector<OutgoingSnapshot> snapshots;
    const uint16_t playerId = cmd.get_player_id();

    if (cmd.get_type() == protocol::ClientOpcode::LOGIN) {
        Jugador* jugador = getJugador(cmd.get_nick());
        nick_to_player_id[cmd.get_nick()] = playerId;
        if (!jugador) {
            // Login de existente: intentar restaurar de persistencia por nick
            bool restaurado = false;

            auto record = PersistenceLoader::load_player_by_nick(
                config.getRutaJugadores(), config.getRutaIndiceJugadores(),
                cmd.get_nick());

            if (record.has_value()) {
                restaurado = restaurarJugadorPersistido(*record);
            }

            if (!restaurado) {
                push_unicast(snapshots, Snapshot::error_message(cmd.get_nick(), "Login fallido: personaje inexistente"), playerId);
                return snapshots;
            }

            jugador = getJugador(cmd.get_nick());

            if (!jugador) {
                 push_unicast(snapshots,Snapshot::error_message(
                    cmd.get_nick(),
                    "Login fallido: error restaurando personaje"), playerId);
                return snapshots;
            }
        }

        player_id_to_nick[cmd.get_player_id()] = cmd.get_nick();

         push_broadcast(snapshots,Snapshot::entity_login(
            cmd.get_nick(), static_cast<uint16_t>(jugador->getMapaId()),
            static_cast<uint16_t>(jugador->getPosX()),
            static_cast<uint16_t>(jugador->getPosY()),
            static_cast<uint8_t>(jugador->getDireccion())));

        // Avisar al cliente en que mapa quedo (persistido)
         push_broadcast(snapshots,Snapshot::map_change(
            cmd.get_nick(), static_cast<uint16_t>(jugador->getMapaId()),
            static_cast<uint16_t>(jugador->getPosX()),
            static_cast<uint16_t>(jugador->getPosY()),
            static_cast<uint8_t>(jugador->getDireccion())));

         push_broadcast(snapshots,
            SnapshotFactory::player_stats_from_player(*jugador));
         push_broadcast(snapshots,
            SnapshotFactory::player_inventory_from_player(*jugador));

        agregarReplayDeJugadores(snapshots, cmd.get_nick(),
                                 jugador->getMapaId(), cmd.get_player_id());
        agregarReplayNpcs(snapshots, jugador->getMapaId(), cmd.get_player_id());
        agregarReplayCriaturas(snapshots, jugador->getMapaId(), cmd.get_player_id());

        return snapshots;
    }

    if (cmd.get_type() == protocol::ClientOpcode::CREATE_CHARACTER) {
        bool creado = agregarJugador(cmd.get_nick(), config.getSpawnMapaId(),
                                     config.getSpawnX(), config.getSpawnY(),
                                     cmd.get_raza(), cmd.get_clase());
        nick_to_player_id[cmd.get_nick()] = playerId;
        if (!creado) {
             push_unicast(snapshots,Snapshot::error_message(
                cmd.get_nick(), "No se pudo crear el personaje"),playerId);
            return snapshots;
        }

        Jugador* jugador = getJugador(cmd.get_nick());

        player_id_to_nick[cmd.get_player_id()] = cmd.get_nick();

         push_broadcast(snapshots,Snapshot::entity_created(
            cmd.get_nick(), static_cast<uint16_t>(jugador->getMapaId()),
            static_cast<uint16_t>(jugador->getPosX()),
            static_cast<uint16_t>(jugador->getPosY()),
            static_cast<uint8_t>(jugador->getDireccion())));

        // Avisar al cliente en que mapa quedo (nuevo)
         push_broadcast(snapshots,Snapshot::map_change(
            cmd.get_nick(), static_cast<uint16_t>(jugador->getMapaId()),
            static_cast<uint16_t>(jugador->getPosX()),
            static_cast<uint16_t>(jugador->getPosY()),
            static_cast<uint8_t>(jugador->getDireccion())));

         push_broadcast(snapshots,
            SnapshotFactory::player_stats_from_player(*jugador));
         push_broadcast(snapshots,
            SnapshotFactory::player_inventory_from_player(*jugador));

        agregarReplayDeJugadores(snapshots, cmd.get_nick(),
                                 jugador->getMapaId(), cmd.get_player_id());
        agregarReplayNpcs(snapshots, jugador->getMapaId(), cmd.get_player_id());
        agregarReplayCriaturas(snapshots, jugador->getMapaId(), cmd.get_player_id());

        return snapshots;
    }

    // -- Disconnect --------------------------
    if (cmd.is_disconnect()) {
        const std::string nombre = getNombreJugadorPorComando(cmd);

        if (!nombre.empty()) {
             push_broadcast(snapshots,Snapshot::entity_remove(nombre));

            removerJugador(nombre);
            player_id_to_nick.erase(cmd.get_player_id());
        }

        return snapshots;
    }

    // -- Comando que requieren jugador --------------------------
    const std::string nombre = getNombreJugadorPorComando(cmd);
    if (nombre.empty()) {
        push_unicast(snapshots, Snapshot::error_message(
            "", "Comando recibido sin jugador asociado"), playerId);
        return snapshots;
    }

    Jugador* jugador = getJugador(nombre);

    // Comandos que un fantasma (vivo == false) NO puede ejecutar
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
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "No puedes hacer eso siendo un fantasma"), playerId);
                return snapshots;
            }
        }
    }

    // Deja de meditar si recibe cualquier comando distinto a MEDITATE mientras
    // se está meditando
    if (jugador && cmd.get_type() != protocol::ClientOpcode::MEDITATE) {
        handle_meditation_interruption(jugador, snapshots, nombre);
    }

    switch (cmd.get_type()) {
        // -- Move --------------------------
        case protocol::ClientOpcode::MOVE: {
            Jugador* jugador = getJugador(nombre);

            if (!jugador) {
                push_unicast(snapshots,
                    Snapshot::error_message(nombre, "Jugador inexistente"), playerId);
                break;
            }

            if (!puedeMoverAhora(nombre)) {
                break;
            }

            int mapaAnterior = jugador->getMapaId();

            bool moved = moverJugador(
                nombre, static_cast<Direccion>(cmd.get_direction()));

            if (!moved) {
                push_unicast(snapshots,
                    Snapshot::error_message(nombre, "No se pudo mover"), playerId);
                break;
            }

            int mapaActual = jugador->getMapaId();

            if (mapaActual != mapaAnterior) {
                push_broadcast(snapshots,Snapshot::map_change(
                    nombre, static_cast<uint16_t>(jugador->getMapaId()),
                    static_cast<uint16_t>(jugador->getPosX()),
                    static_cast<uint16_t>(jugador->getPosY()),
                    static_cast<uint8_t>(jugador->getDireccion())));
                push_broadcast(snapshots,Snapshot::entity_remove(jugador->getNombre()));
                agregarReplayDeJugadores(snapshots, nombre, mapaActual, cmd.get_player_id());
                agregarReplayNpcs(snapshots, mapaActual, cmd.get_player_id());
                agregarReplayCriaturas(snapshots, mapaActual, cmd.get_player_id());
            } else {
                 push_broadcast(snapshots,Snapshot::entity_move(
                    nombre, static_cast<uint16_t>(jugador->getMapaId()),
                    static_cast<uint16_t>(jugador->getPosX()),
                    static_cast<uint16_t>(jugador->getPosY()),
                    static_cast<uint8_t>(jugador->getDireccion())));
            }

            break;
        }
        // -- PICK ITEM --------------------------
        case protocol::ClientOpcode::PICK_ITEM: {
            ResultadoTomarItem resultado =
                tomarItem(nombre, static_cast<int>(cmd.get_item_id()));

            if (!resultado.exito) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "No hay item para recoger"), playerId);
                break;
            }

             push_broadcast(snapshots,Snapshot::item_event(
                static_cast<uint8_t>(protocol::ItemEventAction::PICK), nombre,
                resultado.itemNombre,
                static_cast<uint16_t>(jugador->getMapaId()),
                static_cast<uint16_t>(jugador->getPosX()),
                static_cast<uint16_t>(jugador->getPosY()), resultado.cantidad));

            if (resultado.slotInventario == -1) {
                 push_broadcast(snapshots,
                    SnapshotFactory::player_stats_from_player(*jugador));
            } else {
                 push_broadcast(snapshots,
                    SnapshotFactory::player_inventory_slot_from_player(
                        *jugador, resultado.slotInventario));
            }

            break;
        }

        case protocol::ClientOpcode::DROP_ITEM: {
            int slot = static_cast<int>(cmd.get_slot());

            const auto& slots = jugador->getInventario().getSlots();

            if (slot < 0 || slot >= static_cast<int>(slots.size()) ||
                !slots[slot].has_value()) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "No se pudo arrojar el item"), playerId);
                break;
            }

            std::string itemNombre = slots[slot]->item->getNombre();
            uint16_t cantidad = slots[slot]->cantidad;

            if (tirarItem(nombre, slot)) {
                 push_broadcast(snapshots,
                    SnapshotFactory::player_inventory_slot_from_player(*jugador,
                                                                       slot));

                 push_broadcast(snapshots,Snapshot::item_event(
                    static_cast<uint8_t>(protocol::ItemEventAction::DROP),
                    nombre, itemNombre,
                    static_cast<uint16_t>(jugador->getMapaId()),
                    static_cast<uint16_t>(jugador->getPosX()),
                    static_cast<uint16_t>(jugador->getPosY()), cantidad));
            } else {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "No se pudo arrojar el item"), playerId);
            }

            break;
        }

        // -- ATTACK --------------------------
        case protocol::ClientOpcode::ATTACK: {
            const std::string objetivo = cmd.get_nick();
            ResultadoAtaque resultado = atacar(nombre, objetivo);

            if (!resultado.exito) {
                push_unicast(snapshots,
                    Snapshot::error_message(nombre, "Ataque invalido"), playerId);
                break;
            }
            if (resultado.fueEsquivado) {
                 push_broadcast(snapshots,Snapshot::dodge_event(nombre, objetivo));
                break;
            }

             push_broadcast(snapshots,Snapshot::damage_event(
                nombre, objetivo,
                static_cast<uint16_t>(resultado.danioAplicado),
                resultado.fueCritico));

             push_broadcast(snapshots,
                SnapshotFactory::player_stats_from_player(*jugador));

            if (Jugador* victima = getJugador(objetivo)) {
                 push_broadcast(snapshots,
                    SnapshotFactory::player_stats_from_player(*victima));
            }

            if (resultado.objetivoMurio) {
                 push_broadcast(snapshots,Snapshot::death_event(objetivo));
                 push_broadcast(snapshots,Snapshot::entity_remove(objetivo));

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

                         push_broadcast(snapshots,Snapshot::item_event(
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

                         push_broadcast(snapshots,Snapshot::item_event(
                            static_cast<uint8_t>(
                                protocol::ItemEventAction::DROP),
                            victima->getNombre(), item_defs::ORO,
                            static_cast<uint16_t>(victima->getMapaId()),
                            static_cast<uint16_t>(victima->getPosX()),
                            static_cast<uint16_t>(victima->getPosY()),
                            static_cast<uint16_t>(oroExceso)));
                    }

                     push_broadcast(snapshots,
                        SnapshotFactory::player_stats_from_player(*jugador));

                     push_broadcast(snapshots,
                        SnapshotFactory::player_stats_from_player(*victima));

                     push_broadcast(snapshots,
                        SnapshotFactory::player_inventory_from_player(
                            *victima));
                }
            }
            break;
        }

        // -- MEDITATE --------------------------
        case protocol::ClientOpcode::MEDITATE: {
            if (!jugador || !jugador->estaVivo()) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "No se puede meditar si no estas vivo"), playerId);
                break;
            }
            if (!jugador->getClase()->puedeMeditar()) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "Tu clase no puede meditar"), playerId);
                break;
            }
            if (jugador->estaMeditando()) {
                break;
            }
            jugador->iniciarMeditacion();
             push_broadcast(snapshots,Snapshot::meditation_status(nombre, true));
             push_broadcast(snapshots,
                SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }

            // -- EQUIP ITEM --------------------------
        case protocol::ClientOpcode::EQUIP_ITEM: {
            if (!jugador || !jugador->estaVivo()) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "No puedes equipar items si no estas vivo"), playerId);
                break;
            }

            int slot = static_cast<int>(cmd.get_slot());
            const auto& slots = jugador->getInventario().getSlots();

            if (slot < 0 || slot >= static_cast<int>(slots.size())) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "Slot de inventario invalido"), playerId);
                break;
            }

            if (!slots[slot].has_value()) {
                 push_broadcast(snapshots,
                    Snapshot::error_message(nombre, "Slot vacio"));
                break;
            }

            bool ok = false;

            switch (slots[slot]->item->getTipo()) {
                case TipoItem::ARMA:
                    ok = jugador->equiparArma(slot);
                    break;

                case TipoItem::BACULO:
                    ok = jugador->equiparBaculo(slot);
                    break;

                case TipoItem::ARMADURA:
                    ok = jugador->equiparArmadura(slot);
                    break;

                case TipoItem::CASCO:
                    ok = jugador->equiparCasco(slot);
                    break;

                case TipoItem::ESCUDO:
                    ok = jugador->equiparEscudo(slot);
                    break;

                case TipoItem::POCION:
                    ok = jugador->usarPocion(slot);
                    break;

                default:
                    break;
            }

            if (!ok) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "No se pudo usar/equipar el item"), playerId);
                break;
            }

             push_broadcast(snapshots,
                SnapshotFactory::player_stats_from_player(*jugador));

             push_broadcast(snapshots,
                SnapshotFactory::player_inventory_slot_from_player(*jugador,
                                                                   slot));

            break;
        }

        // -- CHEATS --------------------------
        case protocol::ClientOpcode::CHEAT_GOD: {
            if (!jugador) break;

            bool activo = jugador->toggleCheatVidaInfinita();

             push_broadcast(snapshots,Snapshot::cheat_status(
                nombre, static_cast<uint8_t>(protocol::ClientOpcode::CHEAT_GOD),
                activo));

             push_broadcast(snapshots,
                SnapshotFactory::player_stats_from_player(*jugador));

            break;
        }

        case protocol::ClientOpcode::CHEAT_MANA: {
            if (!jugador) break;

            bool activo = jugador->toggleCheatManaInfinito();

             push_broadcast(snapshots,Snapshot::cheat_status(
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

             push_broadcast(snapshots,Snapshot::cheat_status(
                nombre, static_cast<uint8_t>(protocol::ClientOpcode::CHEAT_DIE),
                !jugador->estaVivo()));

            if (!jugador->estaVivo()) {
                 push_broadcast(snapshots,Snapshot::death_event(nombre));
                 push_broadcast(snapshots,Snapshot::entity_remove(nombre));

                auto items = jugador->soltarTodosLosItems();
                for (auto& item : items) {
                    std::string nombreItem = item.item->getNombre();
                    uint16_t cantidad = item.cantidad;

                    mundo.tirarItem(jugador->getMapaId(), jugador->getPosX(),
                                    jugador->getPosY(), std::move(item));

                     push_broadcast(snapshots,Snapshot::item_event(
                        static_cast<uint8_t>(protocol::ItemEventAction::DROP),
                        jugador->getNombre(), nombreItem,
                        static_cast<uint16_t>(jugador->getMapaId()),
                        static_cast<uint16_t>(jugador->getPosX()),
                        static_cast<uint16_t>(jugador->getPosY()), cantidad));
                }
                int oroExceso = Formulas::calcularOroExceso(
                    jugador->getOro(), jugador->getOroMax());

                if (oroExceso > 0) {
                    jugador->gastarOro(oroExceso);
                    mundo.tirarItem(
                        jugador->getMapaId(), jugador->getPosX(),
                        jugador->getPosY(),
                        SlotInventario(ItemFactory::crearOro(oroExceso)));

                     push_broadcast(snapshots,Snapshot::item_event(
                        static_cast<uint8_t>(protocol::ItemEventAction::DROP),
                        jugador->getNombre(), item_defs::ORO,
                        static_cast<uint16_t>(jugador->getMapaId()),
                        static_cast<uint16_t>(jugador->getPosX()),
                        static_cast<uint16_t>(jugador->getPosY()),
                        static_cast<uint16_t>(oroExceso)));
                }
            }

             push_broadcast(snapshots,
                SnapshotFactory::player_stats_from_player(*jugador));

            break;
        }

        case protocol::ClientOpcode::CHEAT_RESURRECT: {
            if (!jugador) break;

            jugador->revivir(jugador->getVidaMax());

             push_broadcast(snapshots,Snapshot::cheat_status(
                nombre,
                static_cast<uint8_t>(protocol::ClientOpcode::CHEAT_RESURRECT),
                jugador->estaVivo()));

             push_broadcast(snapshots,
                SnapshotFactory::player_stats_from_player(*jugador));

            break;
        }
            // -- RESURRECT --------------------------
        case protocol::ClientOpcode::RESURRECT: {
            if (!jugador || jugador->estaVivo()) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "Solo un fantasma puede usar /resucitar"), playerId);
                break;
            }
            if (jugador->estaResucitando()) {
                 push_broadcast(snapshots,
                    Snapshot::error_message(nombre, "Ya estas resucitando"));
                break;
            }
            InfoNPC destino;
            float distancia;
            if (!encontrarSacerdoteMasCercano(jugador, destino, distancia)) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "No hay ningun sacerdote en el mundo"), playerId);
                break;
            }
            float tiempo = distancia / config.getVelocidadResurreccion();
            jugador->iniciarResurreccion(tiempo, destino.mapaId, destino.x,
                                         destino.y);
            break;
        }

        // -- HEAL --------------------------
        case protocol::ClientOpcode::HEAL: {
            if (!jugador || !jugador->estaVivo()) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "No puedes curarte si eres un fantasma"), playerId);
                break;
            }

            if (!hayNPCCercano(jugador, sacerdotes)) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "Debes estar cerca de un sacerdote para curarte"), playerId);
                break;
            }
            jugador->curar(jugador->getVidaMax());
            jugador->recuperarMana(jugador->getManaMax());
             push_broadcast(snapshots,
                SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }

        // -- BUY ITEM --------------------------
        case protocol::ClientOpcode::BUY_ITEM: {
            bool comercianteCerca = hayNPCCercano(jugador, comerciantes);
            bool sacerdoteCerca = hayNPCCercano(jugador, sacerdotes);

            if (!comercianteCerca && !sacerdoteCerca) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre,
                    "Debes estar cercano a un comerciante o sacerdote para "
                    "comprar"), playerId);
                break;
            }

            const std::string& itemNombre = cmd.get_text();
            int precio = config.getPrecioItem(itemNombre);
            if (precio == 0) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "Ese item no está disponible"), playerId);
                break;
            }

            auto item = crear_item_por_nombre(itemNombre);
            if (!item) {
                 push_broadcast(snapshots,
                    Snapshot::error_message(nombre, "Item desconocido"));
                break;
            }

            // Validar que el NPC cercano vende este tipo de ítem:
            // Sacerdote: báculos y pociones - Comerciante: todo excepto
            // báculos.
            TipoItem tipoItem = item->getTipo();
            bool esBaculo = (tipoItem == TipoItem::BACULO);
            bool esPocion = (tipoItem == TipoItem::POCION);
            bool vendidoPorSacerdote = esBaculo || esPocion;
            bool vendidoPorComerciante = !esBaculo;

            if ((!sacerdoteCerca || !vendidoPorSacerdote) &&
                (!comercianteCerca || !vendidoPorComerciante)) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "El NPC cercano no vende ese tipo de item"), playerId);
                break;
            }

            if (!jugador->gastarOro(precio)) {
                 push_broadcast(snapshots,
                    Snapshot::error_message(nombre, "No tenes suficiente oro"));
                break;
            }

            jugador->agarrarItem(std::move(item));
             push_broadcast(snapshots,
                SnapshotFactory::player_inventory_from_player(*jugador));
             push_broadcast(snapshots,
                SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }

        // -- SELL ITEM --------------------------
        case protocol::ClientOpcode::SELL_ITEM: {
            if (!hayNPCCercano(jugador, comerciantes)) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre,
                    "Debes estar cercano a un comerciante para vender"), playerId);
                break;
            }

            int slot = static_cast<int>(cmd.get_slot());
            const auto& slots = jugador->getInventario().getSlots();
            if (slot < 0 || slot >= static_cast<int>(slots.size())) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "Slot de inventario invalido"), playerId);
                break;
            }

            if (!slots[slot].has_value()) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "Slot de inventario invalido"), playerId);
                break;
            }
            const std::string itemNombre = slots[slot]->item->getNombre();
            int precioVenta = config.getPrecioItem(itemNombre) / 2;
            auto soltado = jugador->soltarItem(slot);
            if (!soltado) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "No se pudo vender el item"), playerId);
                break;
            }
            jugador->agregarOro(precioVenta);
             push_broadcast(snapshots,
                SnapshotFactory::player_inventory_slot_from_player(*jugador,
                                                                   slot));
             push_broadcast(snapshots,
                SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }

        // -- DEPOSIT ITEM --------------------------
        case protocol::ClientOpcode::DEPOSIT_ITEM: {
            if (!hayNPCCercano(jugador, banqueros)) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre,
                    "Debes estar cercano a un banquero para depositar"), playerId);
                break;
            }

            auto& cuentaDeposito =
                cuentasBancarias.try_emplace(nombre, nombre).first->second;
            int slot = static_cast<int>(cmd.get_slot());
            auto soltado = jugador->soltarItem(slot);
            if (!soltado) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "Slot de inventario invalido"), playerId);
                break;
            }
            cuentaDeposito.depositarItem(std::move(*soltado));
             push_broadcast(snapshots,
                SnapshotFactory::player_inventory_from_player(*jugador));
            break;
        }

        // -- DEPOSIT GOLD --------------------------
        case protocol::ClientOpcode::DEPOSIT_GOLD: {
            if (!hayNPCCercano(jugador, banqueros)) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre,
                    "Debes estar cercano a un banquero para depositar"), playerId);
                break;
            }

            int cantidad = static_cast<int>(cmd.get_amount());
            if (!jugador->gastarOro(cantidad)) {
                 push_broadcast(snapshots,
                    Snapshot::error_message(nombre, "No tenes suficiente oro"));
                break;
            }
            cuentasBancarias.try_emplace(nombre, nombre)
                .first->second.depositarOro(cantidad);
             push_broadcast(snapshots,
                SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }

        // -- WITHDRAW ITEM --------------------------
        case protocol::ClientOpcode::WITHDRAW_ITEM: {
            if (!hayNPCCercano(jugador, banqueros)) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "Debes estar cercano a un banquero para retirar"), playerId);
                break;
            }

            auto& cuentaRetiro =
                cuentasBancarias.try_emplace(nombre, nombre).first->second;
            int indice = static_cast<int>(cmd.get_item_id());
            auto slot = cuentaRetiro.retirarItem(indice);
            if (!slot) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "Indice de banco invalido"), playerId);
                break;
            }
            jugador->agarrarItem(std::move(slot->item), slot->cantidad);
             push_broadcast(snapshots,
                SnapshotFactory::player_inventory_from_player(*jugador));
            break;
        }

        // -- WITHDRAW GOLD --------------------------
        case protocol::ClientOpcode::WITHDRAW_GOLD: {
            if (!hayNPCCercano(jugador, banqueros)) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "Debes estar cercano a un banquero para retirar"), playerId);
                break;
            }

            int cantidad = static_cast<int>(cmd.get_amount());
            auto& cuentaRetiro =
                cuentasBancarias.try_emplace(nombre, nombre).first->second;
            if (!cuentaRetiro.retirarOro(cantidad)) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "No tenes suficiente oro en el banco"), playerId);
                break;
            }
            jugador->agregarOro(cantidad);
             push_broadcast(snapshots,
                SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }

        // -- Chat privado --------------------------
        case protocol::ClientOpcode::PRIVATE_MESSAGE: {
            const std::string destino = cmd.get_nick();
            const std::string mensaje = cmd.get_text();
            if (destino.empty()) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "Debe indicar un destinatario"), playerId);
                break;
            }

            if (mensaje.empty()) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "El mensaje no puede estar vacio"), playerId);
                break;
            }

            if (!getJugador(destino)) {
                push_unicast(snapshots,Snapshot::error_message(
                    nombre, "El jugador destinatario no existe"), playerId);
                break;
            }

             push_broadcast(snapshots,
                Snapshot::chat_message(nombre, destino, mensaje));
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
            push_unicast(snapshots,Snapshot::error_message(
                nombre, "Clanes todavia no implementados"), playerId);
            break;

        default:
             push_broadcast(snapshots,
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

// ----------------- Items del piso -----------------

bool Game::tirarItem(const std::string& nombre, int indice, int cantidad) {
    Jugador* jugador = getJugador(nombre);

    if (!jugador || !jugador->estaVivo()) {
        return false;
    }

    auto slot = jugador->soltarItem(indice, cantidad);

    if (!slot) {
        return false;
    }

    mundo.tirarItem(jugador->getMapaId(), jugador->getPosX(),
                    jugador->getPosY(), std::move(*slot));

    return true;
}

ResultadoTomarItem Game::tomarItem(const std::string& nombre, int indice) {
    ResultadoTomarItem resultado;

    Jugador* jugador = getJugador(nombre);

    if (!jugador || !jugador->estaVivo()) {
        return resultado;
    }

    int x = jugador->getPosX();
    int y = jugador->getPosY();
    int mapaId = jugador->getMapaId();

    auto slot_piso = mundo.tomarItemEnPosicion(mapaId, x, y, indice);

    if (!slot_piso) {
        return resultado;
    }

    resultado.itemNombre = slot_piso->item->getNombre();
    resultado.cantidad = static_cast<uint16_t>(slot_piso->cantidad);

    if (slot_piso->item->getTipo() == TipoItem::ORO) {
        jugador->agregarOro(slot_piso->cantidad);
        resultado.exito = true;
        resultado.slotInventario = -1;
        return resultado;
    }

    if (jugador->getInventario().estaLleno()) {
        mundo.tirarItem(mapaId, x, y, std::move(*slot_piso));
        return resultado;
    }

    std::optional<int> slot_inventario =
        jugador->agarrarItem(std::move(slot_piso->item), slot_piso->cantidad);

    if (!slot_inventario.has_value()) {
        mundo.tirarItem(mapaId, x, y, std::move(*slot_piso));
        return resultado;
    }

    resultado.exito = true;
    resultado.slotInventario = *slot_inventario;
    return resultado;
}