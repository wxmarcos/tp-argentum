#include "game/game.h"

#include "common/protocol_defs.h"
#include "game/items/inventario.h"
#include "game/items/item_defs.h"
#include "game/items/itemFactory.h"
#include "game/snapshot_factory.h"

// ----------------- BUY ITEM -----------------

void Game::handleBuyItem(const std::string& nombre, const Command& cmd,
                         std::vector<OutgoingSnapshot>& snapshots, uint16_t playerId) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador) return;

    bool comercianteCerca = hayNPCCercano(jugador, comerciantes);
    bool sacerdoteCerca = hayNPCCercano(jugador, sacerdotes);

    if (!comercianteCerca && !sacerdoteCerca) {
        push_unicast(snapshots, Snapshot::error_message(
            nombre,
            "Debes estar cercano a un comerciante o sacerdote para comprar"), playerId);
        return;
    }

    const std::string& itemNombre = cmd.get_text();
    int precio = config.getPrecioItem(itemNombre);
    if (precio == 0) {
        push_unicast(snapshots, 
            Snapshot::error_message(nombre, "Ese item no está disponible"), playerId);
        return;
    }

    auto item = crear_item_por_nombre(itemNombre);
    if (!item) {
        push_unicast(snapshots, 
            Snapshot::error_message(nombre, "Item desconocido"), playerId);
        return;
    }

    TipoItem tipoItem = item->getTipo();
    bool esBaculo = (tipoItem == TipoItem::BACULO);
    bool esPocion = (tipoItem == TipoItem::POCION);
    bool vendidoPorSacerdote = esBaculo || esPocion;
    bool vendidoPorComerciante = !esBaculo;

    if ((!sacerdoteCerca || !vendidoPorSacerdote) &&
        (!comercianteCerca || !vendidoPorComerciante)) {
        push_unicast(snapshots, Snapshot::error_message(
            nombre, "El NPC cercano no vende ese tipo de item"), playerId);
        return;
    }

    if (!jugador->gastarOro(precio)) {
        push_unicast(snapshots, 
            Snapshot::error_message(nombre, "No tenes suficiente oro"), playerId);
        return;
    }

    jugador->agarrarItem(std::move(item));
    push_broadcast(snapshots, SnapshotFactory::player_inventory_from_player(*jugador));
    push_broadcast(snapshots, SnapshotFactory::player_stats_from_player(*jugador));
}

// ----------------- SELL ITEM -----------------

void Game::handleSellItem(const std::string& nombre, const Command& cmd,
                          std::vector<OutgoingSnapshot>& snapshots, uint16_t playerId) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador) return;

    if (!hayNPCCercano(jugador, comerciantes)) {
        push_unicast(snapshots, Snapshot::error_message(
            nombre, "Debes estar cercano a un comerciante para vender"), playerId);
        return;
    }

    int slot = static_cast<int>(cmd.get_slot());
    const auto& slots = jugador->getInventario().getSlots();
    if (slot < 0 || slot >= static_cast<int>(slots.size())) {
        push_unicast(snapshots, 
            Snapshot::error_message(nombre, "Slot de inventario invalido"), playerId);
        return;
    }
    if (!slots[slot].has_value()) {
        push_unicast(snapshots, 
            Snapshot::error_message(nombre, "Slot de inventario invalido"), playerId);
        return;
    }

    const std::string itemNombre = slots[slot]->item->getNombre();
    int precioVenta = config.getPrecioItem(itemNombre) / 2;
    auto soltado = jugador->soltarItem(slot);
    if (!soltado) {
        push_unicast(snapshots, 
            Snapshot::error_message(nombre, "No se pudo vender el item"), playerId);
        return;
    }

    jugador->agregarOro(precioVenta);
    push_broadcast(snapshots, 
        SnapshotFactory::player_inventory_slot_from_player(*jugador, slot));
    push_broadcast(snapshots, SnapshotFactory::player_stats_from_player(*jugador));
}

// ----------------- DEPOSIT ITEM -----------------

void Game::handleDepositItem(const std::string& nombre, const Command& cmd,
                             std::vector<OutgoingSnapshot>& snapshots, uint16_t playerId) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador) return;

    if (!hayNPCCercano(jugador, banqueros)) {
        push_unicast(snapshots, Snapshot::error_message(
            nombre, "Debes estar cercano a un banquero para depositar"), playerId);
        return;
    }

    auto& cuenta = cuentasBancarias.try_emplace(nombre, nombre).first->second;
    int slot = static_cast<int>(cmd.get_slot());
    auto soltado = jugador->soltarItem(slot);
    if (!soltado) {
        push_unicast(snapshots, 
            Snapshot::error_message(nombre, "Slot de inventario invalido"), playerId);
        return;
    }

    cuenta.depositarItem(std::move(*soltado));
    push_broadcast(snapshots, SnapshotFactory::player_inventory_from_player(*jugador));
}

// ----------------- WITHDRAW ITEM -----------------

void Game::handleWithdrawItem(const std::string& nombre, const Command& cmd,
                              std::vector<OutgoingSnapshot>& snapshots, uint16_t playerId) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador) return;

    if (!hayNPCCercano(jugador, banqueros)) {
        push_unicast(snapshots, Snapshot::error_message(
            nombre, "Debes estar cercano a un banquero para retirar"), playerId);
        return;
    }

    auto& cuenta = cuentasBancarias.try_emplace(nombre, nombre).first->second;
    int indice = static_cast<int>(cmd.get_item_id());
    auto slot = cuenta.retirarItem(indice);
    if (!slot) {
        push_unicast(snapshots, 
            Snapshot::error_message(nombre, "Indice de banco invalido"), playerId);
        return;
    }

    jugador->agarrarItem(std::move(slot->item), slot->cantidad);
    push_broadcast(snapshots, SnapshotFactory::player_inventory_from_player(*jugador));
}

// ----------------- DEPOSIT GOLD -----------------

void Game::handleDepositGold(const std::string& nombre, const Command& cmd,
                             std::vector<OutgoingSnapshot>& snapshots, uint16_t playerId) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador) return;

    if (!hayNPCCercano(jugador, banqueros)) {
        push_unicast(snapshots, Snapshot::error_message(
            nombre, "Debes estar cercano a un banquero para depositar"), playerId);
        return;
    }

    int cantidad = static_cast<int>(cmd.get_amount());
    if (!jugador->gastarOro(cantidad)) {
        push_unicast(snapshots, 
            Snapshot::error_message(nombre, "No tenes suficiente oro"), playerId);
        return;
    }

    cuentasBancarias.try_emplace(nombre, nombre)
        .first->second.depositarOro(cantidad);
    push_broadcast(snapshots, SnapshotFactory::player_stats_from_player(*jugador));
}

// ----------------- WITHDRAW GOLD -----------------

void Game::handleWithdrawGold(const std::string& nombre, const Command& cmd,
                              std::vector<OutgoingSnapshot>& snapshots, uint16_t playerId) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador) return;

    if (!hayNPCCercano(jugador, banqueros)) {
        push_unicast(snapshots, Snapshot::error_message(
            nombre, "Debes estar cercano a un banquero para retirar"), playerId);
        return;
    }

    int cantidad = static_cast<int>(cmd.get_amount());
    auto& cuenta = cuentasBancarias.try_emplace(nombre, nombre).first->second;
    if (!cuenta.retirarOro(cantidad)) {
        push_unicast(snapshots, 
            Snapshot::error_message(nombre, "No tenes suficiente oro en el banco"), playerId);
        return;
    }

    jugador->agregarOro(cantidad);
    push_broadcast(snapshots, SnapshotFactory::player_stats_from_player(*jugador));
}

// ----------------- LIST ITEMS -----------------

void Game::handleListItems(const std::string& nombre,
                           std::vector<OutgoingSnapshot>& snapshots, uint16_t playerId) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador) return;

    const bool comercianteCerca = hayNPCCercano(jugador, comerciantes);
    const bool sacerdoteCerca = hayNPCCercano(jugador, sacerdotes);
    const bool banqueroCerca = hayNPCCercano(jugador, banqueros);

    if (!comercianteCerca && !sacerdoteCerca && !banqueroCerca) {
        push_unicast(snapshots, Snapshot::error_message(
            nombre,
            "Debes estar cerca de un comerciante, sacerdote o banquero"), playerId);
        return;
    }

    if (banqueroCerca) {
        const auto& cuenta =
            cuentasBancarias.try_emplace(nombre, nombre).first->second;
        const auto& items = cuenta.getItems();
        push_broadcast(snapshots, Snapshot::chat_message(
            "Banquero", nombre,
            "Banco — Oro: " + std::to_string(cuenta.getOro())));
        if (items.empty()) {
            push_broadcast(snapshots, Snapshot::chat_message(
                "Banquero", nombre, "No tienes items guardados."));
        } else {
            for (size_t i = 0; i < items.size(); ++i) {
                const auto& slot = items[i];
                std::string linea = "[" + std::to_string(i) + "] " +
                                    slot.item->getNombre();
                if (slot.cantidad > 1)
                    linea += " x" + std::to_string(slot.cantidad);
                push_broadcast(snapshots, 
                    Snapshot::chat_message("Banquero", nombre, linea));
            }
        }
    }

    // Items del comerciante (todo excepto báculos)
    static const std::vector<std::string> itemsComerciante = {
        "espada",        "hacha",          "martillo",
        "arco_simple",   "arco_compuesto",
        "armadura_de_cuero", "armadura_de_placas", "tunica_azul",
        "capucha",       "casco_de_hierro",  "sombrero_magico",
        "escudo_de_tortuga", "escudo_de_hierro",
        "pocion_de_vida", "pocion_de_mana"};

    // Items del sacerdote (báculos y pociones)
    static const std::vector<std::string> itemsSacerdote = {
        "vara_de_fresno", "flauta_elfica", "baculo_nudoso", "baculo_engarzado",
        "pocion_de_vida",  "pocion_de_mana"};

    if (comercianteCerca) {
        push_broadcast(snapshots, 
            Snapshot::chat_message("Comerciante", nombre, "Mis productos:"));
        for (const auto& item : itemsComerciante) {
            int precio = config.getPrecioItem(item);
            if (precio > 0) {
                push_broadcast(snapshots, Snapshot::chat_message(
                    "Comerciante", nombre,
                    "  " + item + " — " + std::to_string(precio) + " oro"));
            }
        }
    }

    if (sacerdoteCerca) {
        push_broadcast(snapshots, 
            Snapshot::chat_message("Sacerdote", nombre, "Mis productos:"));
        for (const auto& item : itemsSacerdote) {
            int precio = config.getPrecioItem(item);
            if (precio > 0) {
                push_broadcast(snapshots, Snapshot::chat_message(
                    "Sacerdote", nombre,
                    "  " + item + " — " + std::to_string(precio) + " oro"));
            }
        }
    }
}
