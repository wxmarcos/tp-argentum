#include "server/persistence/persistence_task_factory.h"

#include "common/protocol_defs.h"
#include "game/items/inventario.h"

static void add_unique_name(std::vector<std::string>& names,
                            const std::string& name) {
    if (name.empty()) {
        return;
    }

    for (const std::string& existing : names) {
        if (existing == name) {
            return;
        }
    }

    names.push_back(name);
}

PersistenceTask PersistenceTaskFactory::from_player(const Jugador& jugador) {
    PersistenceTask task;

    task.nick = jugador.getNombre();

    task.raza = jugador.getRaza()->getNombre();
    task.clase = jugador.getClase()->getNombre();

    task.mapa_id = static_cast<uint16_t>(jugador.getMapaId());
    task.x = static_cast<uint16_t>(jugador.getPosX());
    task.y = static_cast<uint16_t>(jugador.getPosY());
    task.direction = static_cast<uint8_t>(jugador.getDireccion());

    task.nivel = static_cast<uint16_t>(jugador.getNivel());
    task.vida = static_cast<uint16_t>(jugador.getVidaActual());
    task.vida_max = static_cast<uint16_t>(jugador.getVidaMax());

    task.mana = static_cast<uint16_t>(jugador.getManaActual());
    task.mana_max = static_cast<uint16_t>(jugador.getManaMax());

    task.experiencia = static_cast<uint32_t>(jugador.getExperiencia());
    task.oro = static_cast<uint32_t>(jugador.getOro());

    task.constitucion = static_cast<uint16_t>(jugador.getConstitucion());
    task.inteligencia = static_cast<uint16_t>(jugador.getInteligencia());
    task.fuerza = static_cast<uint16_t>(jugador.getFuerza());
    task.agilidad = static_cast<uint16_t>(jugador.getAgilidad());

    const Inventario& inventario = jugador.getInventario();
    const auto& slots = inventario.getSlots();

    for (size_t i = 0; i < slots.size(); ++i) {
        if (!slots[i].has_value()) {
            continue;
        }

        const SlotInventario& slot = *slots[i];

        PersistenceInventoryItem item;

        item.slot_id = static_cast<int>(i);
        item.item = slot.item->getNombre();
        item.cantidad = slot.cantidad;
        item.equipado = inventario.estaEquipado(slot.item.get());

        task.inventario.push_back(item);
    }

    return task;
}

std::vector<std::string> PersistenceTaskFactory::get_affected_players(
    const Command& cmd, const std::string& actor) {
    std::vector<std::string> names;

    switch (cmd.get_type()) {
        case protocol::ClientOpcode::CREATE_CHARACTER:
        case protocol::ClientOpcode::LOGIN:
            add_unique_name(names, cmd.get_nick());
            break;

        case protocol::ClientOpcode::ATTACK:
            add_unique_name(names, actor);
            add_unique_name(names, cmd.get_nick());
            break;

        case protocol::ClientOpcode::MOVE:
        case protocol::ClientOpcode::PICK_ITEM:
        case protocol::ClientOpcode::DROP_ITEM:
        case protocol::ClientOpcode::EQUIP_ITEM:
        case protocol::ClientOpcode::BUY_ITEM:
        case protocol::ClientOpcode::SELL_ITEM:
        case protocol::ClientOpcode::DEPOSIT_ITEM:
        case protocol::ClientOpcode::WITHDRAW_ITEM:
        case protocol::ClientOpcode::DEPOSIT_GOLD:
        case protocol::ClientOpcode::WITHDRAW_GOLD:
        case protocol::ClientOpcode::MEDITATE:
        case protocol::ClientOpcode::RESURRECT:
        case protocol::ClientOpcode::HEAL:
        case protocol::ClientOpcode::DISCONNECT:
            add_unique_name(names, actor);
            break;

        default:
            break;
    }

    return names;
}
