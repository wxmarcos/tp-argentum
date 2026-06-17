#include "game/game.h"

#include "common/protocol_defs.h"
#include "game/items/inventario.h"
#include "game/items/item_defs.h"
#include "game/snapshot_factory.h"

// ----------------- MOVE -----------------

void Game::handleMover(const std::string& nombre, const Command& cmd,
                       std::vector<OutgoingSnapshot>& snapshots,
                       uint16_t playerId) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador) {
        push_unicast(
            snapshots,
            Snapshot::error_message(nombre, "Jugador inexistente"),
            playerId);
        return;
    }

    if (!puedeMoverAhora(nombre)) return;

    int mapaAnterior = jugador->getMapaId();

    bool moved = moverJugador(
        nombre,
        static_cast<Direccion>(cmd.get_direction()));

    if (!moved) {
        push_unicast(
            snapshots,
            Snapshot::error_message(nombre, "No se pudo mover"),
            playerId);
        return;
    }

    int mapaActual = jugador->getMapaId();

    if (mapaActual != mapaAnterior) {
        push_broadcast(
            snapshots,
            Snapshot::entity_remove(jugador->getNombre()));

        push_unicast(
            snapshots,
            Snapshot::map_change(
                nombre,
                static_cast<uint16_t>(jugador->getMapaId()),
                static_cast<uint16_t>(jugador->getPosX()),
                static_cast<uint16_t>(jugador->getPosY()),
                static_cast<uint8_t>(jugador->getDireccion())),
            playerId);

        agregarReplayDeJugadores(snapshots, nombre, mapaActual, playerId);
        agregarReplayNpcs(snapshots, mapaActual, playerId);
        agregarReplayCriaturas(snapshots, mapaActual, playerId);
        agregarReplayItems(snapshots, mapaActual);
    } else {
        push_broadcast(
            snapshots,
            Snapshot::entity_move(
                nombre,
                static_cast<uint16_t>(jugador->getMapaId()),
                static_cast<uint16_t>(jugador->getPosX()),
                static_cast<uint16_t>(jugador->getPosY()),
                static_cast<uint8_t>(jugador->getDireccion())));
    }
}

// ----------------- PICK ITEM -----------------

void Game::handlePickItem(const std::string& nombre, const Command& cmd,
                          std::vector<OutgoingSnapshot>& snapshots, uint16_t playerId) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador) return;

    ResultadoTomarItem resultado =
        tomarItem(nombre, static_cast<int>(cmd.get_item_id()));

    if (!resultado.exito) {
        push_unicast(snapshots, 
            Snapshot::error_message(nombre, "No hay item para recoger"), playerId);
        return;
    }

    push_broadcast(snapshots, Snapshot::item_event(
        static_cast<uint8_t>(protocol::ItemEventAction::PICK), nombre,
        resultado.itemNombre,
        static_cast<uint16_t>(jugador->getMapaId()),
        static_cast<uint16_t>(jugador->getPosX()),
        static_cast<uint16_t>(jugador->getPosY()), resultado.cantidad));

    if (resultado.slotInventario == -1) {
        push_broadcast(snapshots, SnapshotFactory::player_stats_from_player(*jugador));
    } else {
        push_broadcast(snapshots, SnapshotFactory::player_inventory_slot_from_player(
            *jugador, resultado.slotInventario));
    }
}

// ----------------- DROP ITEM -----------------

void Game::handleDropItem(const std::string& nombre, const Command& cmd,
                          std::vector<OutgoingSnapshot>& snapshots, uint16_t playerId) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador) return;

    int slot = static_cast<int>(cmd.get_slot());
    const auto& slots = jugador->getInventario().getSlots();

    if (slot < 0 || slot >= static_cast<int>(slots.size()) ||
        !slots[slot].has_value()) {
        push_unicast(snapshots, 
            Snapshot::error_message(nombre, "No se pudo arrojar el item"), playerId);
        return;
    }

    std::string itemNombre = slots[slot]->item->getNombre();
    uint16_t cantidad = slots[slot]->cantidad;

    if (tirarItem(nombre, slot)) {
        push_broadcast(snapshots, SnapshotFactory::player_inventory_slot_from_player(
            *jugador, slot));
        push_broadcast(snapshots, Snapshot::item_event(
            static_cast<uint8_t>(protocol::ItemEventAction::DROP), nombre,
            itemNombre, static_cast<uint16_t>(jugador->getMapaId()),
            static_cast<uint16_t>(jugador->getPosX()),
            static_cast<uint16_t>(jugador->getPosY()), cantidad));
    } else {
        push_unicast(snapshots, 
            Snapshot::error_message(nombre, "No se pudo arrojar el item"), playerId);
    }
}

// ----------------- EQUIP ITEM -----------------

void Game::handleEquipItem(const std::string& nombre, const Command& cmd,
                           std::vector<OutgoingSnapshot>& snapshots, uint16_t playerId) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador || !jugador->estaVivo()) {
        push_unicast(snapshots, Snapshot::error_message(
            nombre, "No puedes equipar items si no estas vivo"), playerId);
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
        push_unicast(snapshots, Snapshot::error_message(nombre, "Slot vacio"), playerId);
        return;
    }

    bool ok = false;
    switch (slots[slot]->item->getTipo()) {
        case TipoItem::ARMA:    ok = jugador->equiparArma(slot);     break;
        case TipoItem::BACULO:  ok = jugador->equiparBaculo(slot);   break;
        case TipoItem::ARMADURA:ok = jugador->equiparArmadura(slot); break;
        case TipoItem::CASCO:   ok = jugador->equiparCasco(slot);    break;
        case TipoItem::ESCUDO:  ok = jugador->equiparEscudo(slot);   break;
        case TipoItem::POCION:  ok = jugador->usarPocion(slot);      break;
        default: break;
    }

    if (!ok) {
        push_unicast(snapshots, Snapshot::error_message(
            nombre, "No se pudo usar/equipar el item"), playerId);
        return;
    }

    push_broadcast(snapshots, SnapshotFactory::player_stats_from_player(*jugador));
    push_broadcast(snapshots, 
        SnapshotFactory::player_inventory_from_player(*jugador));
}

// ----------------- tirarItem / tomarItem -----------------

bool Game::tirarItem(const std::string& nombre, int indice, int cantidad) {
    Jugador* jugador = getJugador(nombre);
    if (!jugador || !jugador->estaVivo()) return false;

    auto slot = jugador->soltarItem(indice, cantidad);
    if (!slot) return false;

    mundo.tirarItem(jugador->getMapaId(), jugador->getPosX(),
                    jugador->getPosY(), std::move(*slot));
    return true;
}

ResultadoTomarItem Game::tomarItem(const std::string& nombre, int indice) {
    ResultadoTomarItem resultado;

    Jugador* jugador = getJugador(nombre);
    if (!jugador || !jugador->estaVivo()) return resultado;

    int x = jugador->getPosX();
    int y = jugador->getPosY();
    int mapaId = jugador->getMapaId();

    auto slot_piso = mundo.tomarItemEnPosicion(mapaId, x, y, indice);
    if (!slot_piso) return resultado;

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
