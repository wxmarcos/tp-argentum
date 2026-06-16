#include "game/snapshot_factory.h"

Snapshot SnapshotFactory::player_stats_from_player(const Jugador& jugador) {
    return Snapshot::player_stats(
        jugador.getNombre(), jugador.getRaza()->getNombre(),
        jugador.getClase()->getNombre(),
        static_cast<uint16_t>(jugador.getMapaId()),
        static_cast<uint16_t>(jugador.getPosX()),
        static_cast<uint16_t>(jugador.getPosY()),
        static_cast<uint8_t>(jugador.getDireccion()),
        static_cast<uint16_t>(jugador.getNivel()),
        static_cast<uint16_t>(jugador.getVidaActual()),
        static_cast<uint16_t>(jugador.getVidaMax()),
        static_cast<uint16_t>(jugador.getManaActual()),
        static_cast<uint16_t>(jugador.getManaMax()),
        static_cast<uint32_t>(jugador.getExperiencia()),
        static_cast<uint32_t>(jugador.getExpParaSiguienteNivel()),
        static_cast<uint32_t>(jugador.getOro()),
        static_cast<uint16_t>(jugador.getConstitucion()),
        static_cast<uint16_t>(jugador.getInteligencia()),
        static_cast<uint16_t>(jugador.getFuerza()),
        static_cast<uint16_t>(jugador.getAgilidad()));
}

Snapshot SnapshotFactory::player_inventory_from_player(const Jugador& jugador) {
    std::vector<InventorySnapshotItem> items;

    const Inventario& inventario = jugador.getInventario();
    const auto& slots = inventario.getSlots();

    for (size_t i = 0; i < slots.size(); ++i) {
        InventorySnapshotItem item;

        item.slot_id = static_cast<uint16_t>(i);

        if (slots[i].has_value()) {
            const SlotInventario& slot = *slots[i];

            item.item = slot.item->getNombre();
            item.cantidad = static_cast<uint16_t>(slot.cantidad);
            item.equipado = inventario.estaEquipado(slot.item.get());
        } else {
            item.item = "";
            item.cantidad = 0;
            item.equipado = false;
        }

        items.push_back(item);
    }

    return Snapshot::inventory_update(jugador.getNombre(), items);
}

Snapshot SnapshotFactory::player_inventory_slot_from_player(
    const Jugador& jugador, int slot_id) {
    std::vector<InventorySnapshotItem> items;

    const Inventario& inventario = jugador.getInventario();
    const auto& slots = inventario.getSlots();

    if (slot_id < 0 || slot_id >= static_cast<int>(slots.size())) {
        return Snapshot::inventory_update(jugador.getNombre(), items);
    }

    InventorySnapshotItem item;
    item.slot_id = static_cast<uint16_t>(slot_id);
    item.equipado = false;
    item.cantidad = 0;
    item.item = "";

    if (slots[slot_id].has_value()) {
        const SlotInventario& slot = *slots[slot_id];

        item.item = slot.item->getNombre();
        item.cantidad = static_cast<uint16_t>(slot.cantidad);
        item.equipado = inventario.estaEquipado(slot.item.get());
    }

    items.push_back(item);

    return Snapshot::inventory_update(jugador.getNombre(), items);
}
