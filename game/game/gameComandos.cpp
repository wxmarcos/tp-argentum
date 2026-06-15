#include "game.h"

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
std::vector<Snapshot> Game::process(const Command& cmd) {
    std::vector<Snapshot> snapshots;

    if (cmd.get_type() == protocol::ClientOpcode::LOGIN) {
        Jugador* jugador = getJugador(cmd.get_nick());

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

        // Avisar al cliente en que mapa quedo (persistido)
        snapshots.push_back(Snapshot::map_change(
            cmd.get_nick(), static_cast<uint16_t>(jugador->getMapaId()),
            static_cast<uint16_t>(jugador->getPosX()),
            static_cast<uint16_t>(jugador->getPosY()),
            static_cast<uint8_t>(jugador->getDireccion())));

        snapshots.push_back(
            SnapshotFactory::player_stats_from_player(*jugador));
        snapshots.push_back(
            SnapshotFactory::player_inventory_from_player(*jugador));

        agregarReplayDeJugadores(snapshots, cmd.get_nick(),
                                 jugador->getMapaId());
        agregarReplayNpcs(snapshots, jugador->getMapaId());
        agregarReplayCriaturas(snapshots, jugador->getMapaId());

        return snapshots;
    }

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

        snapshots.push_back(Snapshot::entity_created(
            cmd.get_nick(), static_cast<uint16_t>(jugador->getMapaId()),
            static_cast<uint16_t>(jugador->getPosX()),
            static_cast<uint16_t>(jugador->getPosY()),
            static_cast<uint8_t>(jugador->getDireccion())));

        // Avisar al cliente en que mapa quedo (nuevo)
        snapshots.push_back(Snapshot::map_change(
            cmd.get_nick(), static_cast<uint16_t>(jugador->getMapaId()),
            static_cast<uint16_t>(jugador->getPosX()),
            static_cast<uint16_t>(jugador->getPosY()),
            static_cast<uint8_t>(jugador->getDireccion())));

        snapshots.push_back(
            SnapshotFactory::player_stats_from_player(*jugador));
        snapshots.push_back(
            SnapshotFactory::player_inventory_from_player(*jugador));

        agregarReplayDeJugadores(snapshots, cmd.get_nick(),
                                 jugador->getMapaId());
        agregarReplayNpcs(snapshots, jugador->getMapaId());
        agregarReplayCriaturas(snapshots, jugador->getMapaId());

        return snapshots;
    }

    // -- Disconnect --------------------------
    if (cmd.is_disconnect()) {
        const std::string nombre = getNombreJugadorPorComando(cmd);

        if (!nombre.empty()) {
            snapshots.push_back(Snapshot::entity_remove(nombre));

            removerJugador(nombre);
            player_id_to_nick.erase(cmd.get_player_id());
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
                snapshots.push_back(
                    Snapshot::error_message(nombre, "Jugador inexistente"));
                break;
            }

            if (!puedeMoverAhora(nombre)) {
                break;
            }

            int mapaAnterior = jugador->getMapaId();

            bool moved = moverJugador(
                nombre, static_cast<Direccion>(cmd.get_direction()));

            if (!moved) {
                snapshots.push_back(
                    Snapshot::error_message(nombre, "No se pudo mover"));
                break;
            }

            int mapaActual = jugador->getMapaId();

            if (mapaActual != mapaAnterior) {
                snapshots.push_back(Snapshot::map_change(
                    nombre, static_cast<uint16_t>(jugador->getMapaId()),
                    static_cast<uint16_t>(jugador->getPosX()),
                    static_cast<uint16_t>(jugador->getPosY()),
                    static_cast<uint8_t>(jugador->getDireccion())));
                snapshots.push_back(Snapshot::entity_remove(jugador->getNombre()));
                agregarReplayDeJugadores(snapshots, nombre, mapaActual);
                agregarReplayNpcs(snapshots, mapaActual);
                agregarReplayCriaturas(snapshots, mapaActual);
            } else {
                snapshots.push_back(Snapshot::entity_move(
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
                snapshots.push_back(Snapshot::error_message(
                    nombre, "No hay item para recoger"));
                break;
            }

            snapshots.push_back(Snapshot::item_event(
                static_cast<uint8_t>(protocol::ItemEventAction::PICK), nombre,
                resultado.itemNombre,
                static_cast<uint16_t>(jugador->getMapaId()),
                static_cast<uint16_t>(jugador->getPosX()),
                static_cast<uint16_t>(jugador->getPosY()), resultado.cantidad));

            if (resultado.slotInventario == -1) {
                snapshots.push_back(
                    SnapshotFactory::player_stats_from_player(*jugador));
            } else {
                snapshots.push_back(
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
                snapshots.push_back(Snapshot::error_message(
                    nombre, "No se pudo arrojar el item"));
                break;
            }

            std::string itemNombre = slots[slot]->item->getNombre();
            uint16_t cantidad = slots[slot]->cantidad;

            if (tirarItem(nombre, slot)) {
                snapshots.push_back(
                    SnapshotFactory::player_inventory_slot_from_player(*jugador,
                                                                       slot));

                snapshots.push_back(Snapshot::item_event(
                    static_cast<uint8_t>(protocol::ItemEventAction::DROP),
                    nombre, itemNombre,
                    static_cast<uint16_t>(jugador->getMapaId()),
                    static_cast<uint16_t>(jugador->getPosX()),
                    static_cast<uint16_t>(jugador->getPosY()), cantidad));
            } else {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "No se pudo arrojar el item"));
            }

            break;
        }

        // -- ATTACK --------------------------
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
                break;
            }

            snapshots.push_back(Snapshot::damage_event(
                nombre, objetivo,
                static_cast<uint16_t>(resultado.danioAplicado),
                resultado.fueCritico));

            snapshots.push_back(
                SnapshotFactory::player_stats_from_player(*jugador));

            if (Jugador* victima = getJugador(objetivo)) {
                snapshots.push_back(
                    SnapshotFactory::player_stats_from_player(*victima));
            }

            if (resultado.objetivoMurio) {
                snapshots.push_back(Snapshot::death_event(objetivo));
                snapshots.push_back(Snapshot::entity_remove(objetivo));

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
                        SnapshotFactory::player_inventory_from_player(
                            *victima));
                }
            }
            break;
        }

        // -- MEDITATE --------------------------
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
            if (jugador->estaMeditando()) {
                break;
            }
            jugador->iniciarMeditacion();
            snapshots.push_back(Snapshot::meditation_status(nombre, true));
            snapshots.push_back(
                SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }

            // -- EQUIP ITEM --------------------------
        case protocol::ClientOpcode::EQUIP_ITEM: {
            if (!jugador || !jugador->estaVivo()) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "No puedes equipar items si no estas vivo"));
                break;
            }

            int slot = static_cast<int>(cmd.get_slot());
            const auto& slots = jugador->getInventario().getSlots();

            if (slot < 0 || slot >= static_cast<int>(slots.size())) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Slot de inventario invalido"));
                break;
            }

            if (!slots[slot].has_value()) {
                snapshots.push_back(
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
                snapshots.push_back(Snapshot::error_message(
                    nombre, "No se pudo usar/equipar el item"));
                break;
            }

            snapshots.push_back(
                SnapshotFactory::player_stats_from_player(*jugador));

            snapshots.push_back(
                SnapshotFactory::player_inventory_slot_from_player(*jugador,
                                                                   slot));

            break;
        }

        // -- CHEATS --------------------------
        case protocol::ClientOpcode::CHEAT_GOD: {
            if (!jugador) break;

            bool activo = jugador->toggleCheatVidaInfinita();

            snapshots.push_back(Snapshot::cheat_status(
                nombre, static_cast<uint8_t>(protocol::ClientOpcode::CHEAT_GOD),
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
                nombre, static_cast<uint8_t>(protocol::ClientOpcode::CHEAT_DIE),
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
                int oroExceso = Formulas::calcularOroExceso(
                    jugador->getOro(), jugador->getOroMax());

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
            // -- RESURRECT --------------------------
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

        // -- HEAL --------------------------
        case protocol::ClientOpcode::HEAL: {
            if (!jugador || !jugador->estaVivo()) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "No puedes curarte si eres un fantasma"));
                break;
            }

            if (!hayNPCCercano(jugador, sacerdotes)) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Debes estar cerca de un sacerdote para curarte"));
                break;
            }
            jugador->curar(jugador->getVidaMax());
            jugador->recuperarMana(jugador->getManaMax());
            snapshots.push_back(
                SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }

        // -- BUY ITEM --------------------------
        case protocol::ClientOpcode::BUY_ITEM: {
            bool comercianteCerca = hayNPCCercano(jugador, comerciantes);
            bool sacerdoteCerca = hayNPCCercano(jugador, sacerdotes);

            if (!comercianteCerca && !sacerdoteCerca) {
                snapshots.push_back(Snapshot::error_message(
                    nombre,
                    "Debes estar cercano a un comerciante o sacerdote para "
                    "comprar"));
                break;
            }

            const std::string& itemNombre = cmd.get_text();
            int precio = config.getPrecioItem(itemNombre);
            if (precio == 0) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Ese item no está disponible"));
                break;
            }

            auto item = crear_item_por_nombre(itemNombre);
            if (!item) {
                snapshots.push_back(
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
                snapshots.push_back(Snapshot::error_message(
                    nombre, "El NPC cercano no vende ese tipo de item"));
                break;
            }

            if (!jugador->gastarOro(precio)) {
                snapshots.push_back(
                    Snapshot::error_message(nombre, "No tenes suficiente oro"));
                break;
            }

            jugador->agarrarItem(std::move(item));
            snapshots.push_back(
                SnapshotFactory::player_inventory_from_player(*jugador));
            snapshots.push_back(
                SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }

        // -- SELL ITEM --------------------------
        case protocol::ClientOpcode::SELL_ITEM: {
            if (!hayNPCCercano(jugador, comerciantes)) {
                snapshots.push_back(Snapshot::error_message(
                    nombre,
                    "Debes estar cercano a un comerciante para vender"));
                break;
            }

            int slot = static_cast<int>(cmd.get_slot());
            const auto& slots = jugador->getInventario().getSlots();
            if (slot < 0 || slot >= static_cast<int>(slots.size())) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Slot de inventario invalido"));
                break;
            }

            if (!slots[slot].has_value()) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Slot de inventario invalido"));
                break;
            }
            const std::string itemNombre = slots[slot]->item->getNombre();
            int precioVenta = config.getPrecioItem(itemNombre) / 2;
            auto soltado = jugador->soltarItem(slot);
            if (!soltado) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "No se pudo vender el item"));
                break;
            }
            jugador->agregarOro(precioVenta);
            snapshots.push_back(
                SnapshotFactory::player_inventory_slot_from_player(*jugador,
                                                                   slot));
            snapshots.push_back(
                SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }

        // -- DEPOSIT ITEM --------------------------
        case protocol::ClientOpcode::DEPOSIT_ITEM: {
            if (!hayNPCCercano(jugador, banqueros)) {
                snapshots.push_back(Snapshot::error_message(
                    nombre,
                    "Debes estar cercano a un banquero para depositar"));
                break;
            }

            auto& cuentaDeposito =
                cuentasBancarias.try_emplace(nombre, nombre).first->second;
            int slot = static_cast<int>(cmd.get_slot());
            auto soltado = jugador->soltarItem(slot);
            if (!soltado) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Slot de inventario invalido"));
                break;
            }
            cuentaDeposito.depositarItem(std::move(*soltado));
            snapshots.push_back(
                SnapshotFactory::player_inventory_from_player(*jugador));
            break;
        }

        // -- DEPOSIT GOLD --------------------------
        case protocol::ClientOpcode::DEPOSIT_GOLD: {
            if (!hayNPCCercano(jugador, banqueros)) {
                snapshots.push_back(Snapshot::error_message(
                    nombre,
                    "Debes estar cercano a un banquero para depositar"));
                break;
            }

            int cantidad = static_cast<int>(cmd.get_amount());
            if (!jugador->gastarOro(cantidad)) {
                snapshots.push_back(
                    Snapshot::error_message(nombre, "No tenes suficiente oro"));
                break;
            }
            cuentasBancarias.try_emplace(nombre, nombre)
                .first->second.depositarOro(cantidad);
            snapshots.push_back(
                SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }

        // -- WITHDRAW ITEM --------------------------
        case protocol::ClientOpcode::WITHDRAW_ITEM: {
            if (!hayNPCCercano(jugador, banqueros)) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Debes estar cercano a un banquero para retirar"));
                break;
            }

            auto& cuentaRetiro =
                cuentasBancarias.try_emplace(nombre, nombre).first->second;
            int indice = static_cast<int>(cmd.get_item_id());
            auto slot = cuentaRetiro.retirarItem(indice);
            if (!slot) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Indice de banco invalido"));
                break;
            }
            jugador->agarrarItem(std::move(slot->item), slot->cantidad);
            snapshots.push_back(
                SnapshotFactory::player_inventory_from_player(*jugador));
            break;
        }

        // -- WITHDRAW GOLD --------------------------
        case protocol::ClientOpcode::WITHDRAW_GOLD: {
            if (!hayNPCCercano(jugador, banqueros)) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Debes estar cercano a un banquero para retirar"));
                break;
            }

            int cantidad = static_cast<int>(cmd.get_amount());
            auto& cuentaRetiro =
                cuentasBancarias.try_emplace(nombre, nombre).first->second;
            if (!cuentaRetiro.retirarOro(cantidad)) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "No tenes suficiente oro en el banco"));
                break;
            }
            jugador->agregarOro(cantidad);
            snapshots.push_back(
                SnapshotFactory::player_stats_from_player(*jugador));
            break;
        }

        // -- Chat privado --------------------------
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

        // -- Clanes -----------------------------------------
        case protocol::ClientOpcode::CLAN_CREATE: {
            std::string clanNom = cmd.get_clan_name();
            Jugador* j = getJugador(nombre);
            if (!j) break;
            if (clanNom.empty()) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "El nombre del clan no puede ser vacio"));
                break;
            }
            if (j->estaEnClan()) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Ya perteneces a un clan"));
                break;
            }
            if (clanes.count(clanNom)) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Ya existe un clan con ese nombre"));
                break;
            }
            clanes.emplace(clanNom, Clan(clanNom, nombre));
            j->setClanNombre(clanNom);
            snapshots.push_back(Snapshot::chat_message(
                "Sistema", nombre,
                "Clan '" + clanNom + "' creado. Eres el fundador."));
            break;
        }

        case protocol::ClientOpcode::CLAN_JOIN: {
            std::string clanNom = cmd.get_clan_name();
            Jugador* j = getJugador(nombre);
            if (!j) break;
            if (j->estaEnClan()) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Ya perteneces a un clan"));
                break;
            }
            auto it = clanes.find(clanNom);
            if (it == clanes.end()) {
                snapshots.push_back(
                    Snapshot::error_message(nombre, "El clan no existe"));
                break;
            }
            Clan& clan = it->second;
            if (clan.esMiembro(nombre)) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Ya eres miembro de ese clan"));
                break;
            }
            if (clan.hayPendiente(nombre)) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Ya tienes una solicitud pendiente en ese clan"));
                break;
            }
            clan.agregarSolicitud(nombre);
            snapshots.push_back(Snapshot::chat_message(
                "Sistema", nombre,
                "Solicitud enviada al clan '" + clanNom + "'"));
            // Notificar al fundador si está online
            const std::string& fundador = clan.getFundador();
            if (jugadores.count(fundador)) {
                snapshots.push_back(Snapshot::chat_message(
                    "Sistema", fundador,
                    nombre + " quiere unirse a tu clan."));
            }
            break;
        }

        case protocol::ClientOpcode::CLAN_REVIEW: {
            Jugador* j = getJugador(nombre);
            if (!j || !j->estaEnClan()) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "No perteneces a un clan"));
                break;
            }
            auto it = clanes.find(j->getClanNombre());
            if (it == clanes.end()) break;
            Clan& clan = it->second;
            if (clan.getFundador() != nombre) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Solo el fundador puede ver las solicitudes"));
                break;
            }
            const auto& sols = clan.getSolicitudes();
            if (sols.empty()) {
                snapshots.push_back(Snapshot::chat_message(
                    "Sistema", nombre, "No hay solicitudes pendientes."));
            } else {
                std::string lista = "Solicitudes pendientes: ";
                for (size_t i = 0; i < sols.size(); ++i) {
                    if (i > 0) lista += ", ";
                    lista += sols[i];
                }
                snapshots.push_back(
                    Snapshot::chat_message("Sistema", nombre, lista));
            }
            break;
        }

        case protocol::ClientOpcode::CLAN_ACCEPT: {
            std::string nickSol = cmd.get_nick();
            Jugador* j = getJugador(nombre);
            if (!j || !j->estaEnClan()) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "No perteneces a un clan"));
                break;
            }
            auto it = clanes.find(j->getClanNombre());
            if (it == clanes.end()) break;
            Clan& clan = it->second;
            if (clan.getFundador() != nombre) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Solo el fundador puede aceptar solicitudes"));
                break;
            }
            if (!clan.hayPendiente(nickSol)) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "No hay solicitud pendiente de " + nickSol));
                break;
            }
            clan.aprobarSolicitud(nickSol);
            Jugador* ingresante = getJugador(nickSol);
            if (ingresante) {
                ingresante->setClanNombre(j->getClanNombre());
                snapshots.push_back(Snapshot::chat_message(
                    "Sistema", nickSol,
                    "Tu solicitud al clan '" + j->getClanNombre() +
                        "' fue aceptada!"));
            }
            snapshots.push_back(Snapshot::chat_message(
                "Sistema", nombre, nickSol + " ahora es miembro del clan."));
            break;
        }

        case protocol::ClientOpcode::CLAN_REJECT: {
            std::string nickSol = cmd.get_nick();
            Jugador* j = getJugador(nombre);
            if (!j || !j->estaEnClan()) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "No perteneces a un clan"));
                break;
            }
            auto it = clanes.find(j->getClanNombre());
            if (it == clanes.end()) break;
            Clan& clan = it->second;
            if (clan.getFundador() != nombre) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Solo el fundador puede rechazar solicitudes"));
                break;
            }
            if (!clan.rechazarSolicitud(nickSol)) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "No hay solicitud pendiente de " + nickSol));
                break;
            }
            Jugador* solicitante = getJugador(nickSol);
            if (solicitante) {
                snapshots.push_back(Snapshot::chat_message(
                    "Sistema", nickSol,
                    "Tu solicitud al clan '" + j->getClanNombre() +
                        "' fue rechazada."));
            }
            snapshots.push_back(Snapshot::chat_message(
                "Sistema", nombre,
                "Solicitud de " + nickSol + " rechazada."));
            break;
        }

        case protocol::ClientOpcode::CLAN_BAN:
        case protocol::ClientOpcode::CLAN_KICK: {
            std::string nickTarget = cmd.get_nick();
            Jugador* j = getJugador(nombre);
            if (!j || !j->estaEnClan()) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "No perteneces a un clan"));
                break;
            }
            auto it = clanes.find(j->getClanNombre());
            if (it == clanes.end()) break;
            Clan& clan = it->second;
            if (clan.getFundador() != nombre) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "Solo el fundador puede expulsar miembros"));
                break;
            }
            if (nickTarget == nombre) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "No puedes expulsarte a ti mismo"));
                break;
            }
            if (!clan.removerMiembro(nickTarget)) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, nickTarget + " no es miembro del clan"));
                break;
            }
            Jugador* expulsado = getJugador(nickTarget);
            if (expulsado) {
                expulsado->setClanNombre("");
                snapshots.push_back(Snapshot::chat_message(
                    "Sistema", nickTarget,
                    "Fuiste expulsado del clan '" + j->getClanNombre() +
                        "'."));
            }
            snapshots.push_back(Snapshot::chat_message(
                "Sistema", nombre, nickTarget + " fue expulsado del clan."));
            break;
        }

        case protocol::ClientOpcode::CLAN_LEAVE: {
            Jugador* j = getJugador(nombre);
            if (!j || !j->estaEnClan()) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "No perteneces a un clan"));
                break;
            }
            auto it = clanes.find(j->getClanNombre());
            if (it == clanes.end()) {
                j->setClanNombre("");
                break;
            }
            Clan& clan = it->second;
            if (clan.getFundador() == nombre) {
                snapshots.push_back(Snapshot::error_message(
                    nombre, "El fundador no puede abandonar el clan"));
                break;
            }
            clan.removerMiembro(nombre);
            std::string clanNom = j->getClanNombre();
            j->setClanNombre("");
            snapshots.push_back(Snapshot::chat_message(
                "Sistema", nombre, "Abandonaste el clan '" + clanNom + "'."));
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
