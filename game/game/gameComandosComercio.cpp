#include "game/game.h"

#include "common/protocol_defs.h"
#include "game/items/inventario.h"
#include "game/items/item_defs.h"
#include "game/snapshot_factory.h"

// ----------------- BUY ITEM -----------------

void Game::handleBuyItem(const std::string& nombre, const Command& cmd,
                         std::vector<Snapshot>& snapshots) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador) return;

    bool comercianteCerca = hayNPCCercano(jugador, comerciantes);
    bool sacerdoteCerca = hayNPCCercano(jugador, sacerdotes);

    if (!comercianteCerca && !sacerdoteCerca) {
        snapshots.push_back(Snapshot::error_message(
            nombre,
            "Debes estar cercano a un comerciante o sacerdote para comprar"));
        return;
    }

    const std::string& itemNombre = cmd.get_text();
    int precio = config.getPrecioItem(itemNombre);
    if (precio == 0) {
        snapshots.push_back(
            Snapshot::error_message(nombre, "Ese item no está disponible"));
        return;
    }

    auto item = crear_item_por_nombre(itemNombre);
    if (!item) {
        snapshots.push_back(
            Snapshot::error_message(nombre, "Item desconocido"));
        return;
    }

    TipoItem tipoItem = item->getTipo();
    bool esBaculo = (tipoItem == TipoItem::BACULO);
    bool esPocion = (tipoItem == TipoItem::POCION);
    bool vendidoPorSacerdote = esBaculo || esPocion;
    bool vendidoPorComerciante = !esBaculo;

    if ((!sacerdoteCerca || !vendidoPorSacerdote) &&
        (!comercianteCerca || !vendidoPorComerciante)) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "El NPC cercano no vende ese tipo de item"));
        return;
    }

    if (!jugador->gastarOro(precio)) {
        snapshots.push_back(
            Snapshot::error_message(nombre, "No tenes suficiente oro"));
        return;
    }

    jugador->agarrarItem(std::move(item));
    snapshots.push_back(SnapshotFactory::player_inventory_from_player(*jugador));
    snapshots.push_back(SnapshotFactory::player_stats_from_player(*jugador));
}

// ----------------- SELL ITEM -----------------

void Game::handleSellItem(const std::string& nombre, const Command& cmd,
                          std::vector<Snapshot>& snapshots) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador) return;

    if (!hayNPCCercano(jugador, comerciantes)) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "Debes estar cercano a un comerciante para vender"));
        return;
    }

    int slot = static_cast<int>(cmd.get_slot());
    const auto& slots = jugador->getInventario().getSlots();
    if (slot < 0 || slot >= static_cast<int>(slots.size())) {
        snapshots.push_back(
            Snapshot::error_message(nombre, "Slot de inventario invalido"));
        return;
    }
    if (!slots[slot].has_value()) {
        snapshots.push_back(
            Snapshot::error_message(nombre, "Slot de inventario invalido"));
        return;
    }

    const std::string itemNombre = slots[slot]->item->getNombre();
    int precioVenta = config.getPrecioItem(itemNombre) / 2;
    auto soltado = jugador->soltarItem(slot);
    if (!soltado) {
        snapshots.push_back(
            Snapshot::error_message(nombre, "No se pudo vender el item"));
        return;
    }

    jugador->agregarOro(precioVenta);
    snapshots.push_back(
        SnapshotFactory::player_inventory_slot_from_player(*jugador, slot));
    snapshots.push_back(SnapshotFactory::player_stats_from_player(*jugador));
}

// ----------------- DEPOSIT ITEM -----------------

void Game::handleDepositItem(const std::string& nombre, const Command& cmd,
                             std::vector<Snapshot>& snapshots) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador) return;

    if (!hayNPCCercano(jugador, banqueros)) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "Debes estar cercano a un banquero para depositar"));
        return;
    }

    auto& cuenta = cuentasBancarias.try_emplace(nombre, nombre).first->second;
    int slot = static_cast<int>(cmd.get_slot());
    auto soltado = jugador->soltarItem(slot);
    if (!soltado) {
        snapshots.push_back(
            Snapshot::error_message(nombre, "Slot de inventario invalido"));
        return;
    }

    cuenta.depositarItem(std::move(*soltado));
    snapshots.push_back(SnapshotFactory::player_inventory_from_player(*jugador));
}

// ----------------- WITHDRAW ITEM -----------------

void Game::handleWithdrawItem(const std::string& nombre, const Command& cmd,
                              std::vector<Snapshot>& snapshots) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador) return;

    if (!hayNPCCercano(jugador, banqueros)) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "Debes estar cercano a un banquero para retirar"));
        return;
    }

    auto& cuenta = cuentasBancarias.try_emplace(nombre, nombre).first->second;
    int indice = static_cast<int>(cmd.get_item_id());
    auto slot = cuenta.retirarItem(indice);
    if (!slot) {
        snapshots.push_back(
            Snapshot::error_message(nombre, "Indice de banco invalido"));
        return;
    }

    jugador->agarrarItem(std::move(slot->item), slot->cantidad);
    snapshots.push_back(SnapshotFactory::player_inventory_from_player(*jugador));
}

// ----------------- DEPOSIT GOLD -----------------

void Game::handleDepositGold(const std::string& nombre, const Command& cmd,
                             std::vector<Snapshot>& snapshots) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador) return;

    if (!hayNPCCercano(jugador, banqueros)) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "Debes estar cercano a un banquero para depositar"));
        return;
    }

    int cantidad = static_cast<int>(cmd.get_amount());
    if (!jugador->gastarOro(cantidad)) {
        snapshots.push_back(
            Snapshot::error_message(nombre, "No tenes suficiente oro"));
        return;
    }

    cuentasBancarias.try_emplace(nombre, nombre)
        .first->second.depositarOro(cantidad);
    snapshots.push_back(SnapshotFactory::player_stats_from_player(*jugador));
}

// ----------------- WITHDRAW GOLD -----------------

void Game::handleWithdrawGold(const std::string& nombre, const Command& cmd,
                              std::vector<Snapshot>& snapshots) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador) return;

    if (!hayNPCCercano(jugador, banqueros)) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "Debes estar cercano a un banquero para retirar"));
        return;
    }

    int cantidad = static_cast<int>(cmd.get_amount());
    auto& cuenta = cuentasBancarias.try_emplace(nombre, nombre).first->second;
    if (!cuenta.retirarOro(cantidad)) {
        snapshots.push_back(
            Snapshot::error_message(nombre, "No tenes suficiente oro en el banco"));
        return;
    }

    jugador->agregarOro(cantidad);
    snapshots.push_back(SnapshotFactory::player_stats_from_player(*jugador));
}
