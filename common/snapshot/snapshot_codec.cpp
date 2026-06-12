#include "common/snapshot/snapshot_codec.h"

#include <stdexcept>

#include "common/network/socket_helpers.h"

static void validate_payload_size(uint16_t payload_size, uint16_t expected,
                                  const char* error_prefix) {
    if (payload_size != expected) {
        throw std::runtime_error(std::string(error_prefix) +
                                 " payload_size invalido");
    }
}

static uint16_t position_payload_size(const Snapshot& snapshot) {
    return static_cast<uint16_t>(sizeof(uint16_t) + snapshot.get_nick().size() +
                                 sizeof(uint16_t) +  // mapa_id
                                 sizeof(uint16_t) +  // x
                                 sizeof(uint16_t) +  // y
                                 sizeof(uint8_t));   // direction
}

static uint16_t damage_payload_size(const Snapshot& snapshot) {
    return static_cast<uint16_t>(
        sizeof(uint16_t) + snapshot.get_attacker().size() + sizeof(uint16_t) +
        snapshot.get_target().size() + sizeof(uint16_t) + sizeof(uint8_t));
}

static uint16_t dodge_payload_size(const Snapshot& snapshot) {
    return static_cast<uint16_t>(
        sizeof(uint16_t) + snapshot.get_attacker().size() + sizeof(uint16_t) +
        snapshot.get_target().size());
}

static uint16_t death_payload_size(const Snapshot& snapshot) {
    return static_cast<uint16_t>(sizeof(uint16_t) +
                                 snapshot.get_target().size());
}

static uint16_t chat_payload_size(const Snapshot& snapshot) {
    return static_cast<uint16_t>(sizeof(uint16_t) + snapshot.get_nick().size() +
                                 sizeof(uint16_t) +
                                 snapshot.get_target().size() +
                                 sizeof(uint16_t) + snapshot.get_text().size());
}

static uint16_t player_stats_payload_size(const Snapshot& snapshot) {
    return static_cast<uint16_t>(
        sizeof(uint16_t) + snapshot.get_nick().size() + sizeof(uint16_t) +
        snapshot.get_raza().size() + sizeof(uint16_t) +
        snapshot.get_clase().size() + sizeof(uint16_t) + sizeof(uint16_t) +
        sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint16_t) +
        sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t) +
        sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint32_t) +
        sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t) +
        sizeof(uint16_t));
}

static uint16_t item_event_payload_size(const Snapshot& snapshot) {
    return static_cast<uint16_t>(sizeof(uint8_t) + sizeof(uint16_t) +
                                 snapshot.get_nick().size() + sizeof(uint16_t) +
                                 snapshot.get_item_name().size() +
                                 sizeof(uint16_t) +  // mapa_id
                                 sizeof(uint16_t) +  // x
                                 sizeof(uint16_t) +  // y
                                 sizeof(uint16_t));  // amount
}

static uint16_t inventory_update_payload_size(const Snapshot& snapshot) {
    uint16_t size = 0;

    size += sizeof(uint16_t) + snapshot.get_nick().size();
    size += sizeof(uint16_t);

    for (const auto& item : snapshot.get_inventory_items()) {
        size += sizeof(uint16_t);
        size += sizeof(uint16_t) + item.item.size();
        size += sizeof(uint16_t);
        size += sizeof(uint8_t);
    }

    return size;
}
static uint16_t error_payload_size(const Snapshot& snapshot) {
    return static_cast<uint16_t>(sizeof(uint16_t) + snapshot.get_nick().size() +
                                 sizeof(uint16_t) + snapshot.get_text().size());
}

static uint16_t meditation_status_payload_size(const Snapshot& snapshot) {
    return static_cast<uint16_t>(sizeof(uint16_t) + snapshot.get_nick().size() +
                                 sizeof(uint8_t));
}

static uint16_t cheat_status_payload_size(const Snapshot& snapshot) {
    return static_cast<uint16_t>(sizeof(uint16_t) + snapshot.get_nick().size() +
                                 sizeof(uint8_t) + sizeof(uint8_t));
}
static uint16_t map_change_payload_size(const Snapshot& snapshot) {
    return static_cast<uint16_t>(sizeof(uint16_t) + snapshot.get_nick().size() +
                                 sizeof(uint16_t) +  // mapa_id
                                 sizeof(uint16_t) +  // x
                                 sizeof(uint16_t) +  // y
                                 sizeof(uint8_t));   // direction
}

uint16_t snapshot_payload_size(const Snapshot& snapshot) {
    switch (snapshot.get_opcode()) {
        case protocol::ServerOpcode::ENTITY_CREATED:
        case protocol::ServerOpcode::ENTITY_LOGIN:
        case protocol::ServerOpcode::ENTITY_MOVE:
            return position_payload_size(snapshot);

        case protocol::ServerOpcode::ENTITY_REMOVE:
            return static_cast<uint16_t>(sizeof(uint16_t) +
                                         snapshot.get_nick().size());

        case protocol::ServerOpcode::DAMAGE_EVENT:
            return damage_payload_size(snapshot);

        case protocol::ServerOpcode::DODGE_EVENT:
            return dodge_payload_size(snapshot);

        case protocol::ServerOpcode::DEATH_EVENT:
            return death_payload_size(snapshot);
        case protocol::ServerOpcode::ITEM_EVENT:
            return item_event_payload_size(snapshot);
        case protocol::ServerOpcode::CHAT_MESSAGE:
            return chat_payload_size(snapshot);

        case protocol::ServerOpcode::PLAYER_STATS:
            return player_stats_payload_size(snapshot);
        case protocol::ServerOpcode::INVENTORY_UPDATE:
            return inventory_update_payload_size(snapshot);
        case protocol::ServerOpcode::MEDITATION_STATUS:
            return meditation_status_payload_size(snapshot);
        case protocol::ServerOpcode::CHEAT_STATUS:
            return cheat_status_payload_size(snapshot);
        case protocol::ServerOpcode::MAP_CHANGE:
            return map_change_payload_size(snapshot);

        case protocol::ServerOpcode::ERROR_MESSAGE:
            return error_payload_size(snapshot);

        default:
            throw std::runtime_error("Snapshot::send opcode no soportado");
    }
}

static void send_position_snapshot(Socket& socket, const Snapshot& snapshot) {
    send_string(socket, snapshot.get_nick());
    send_u16(socket, snapshot.get_mapa_id());
    send_u16(socket, snapshot.get_x());
    send_u16(socket, snapshot.get_y());
    send_u8(socket, snapshot.get_direction());
}

static void send_entity_remove(Socket& socket, const Snapshot& snapshot) {
    send_string(socket, snapshot.get_nick());
}

static void send_damage_event(Socket& socket, const Snapshot& snapshot) {
    send_string(socket, snapshot.get_attacker());
    send_string(socket, snapshot.get_target());
    send_u16(socket, snapshot.get_damage());
    send_u8(socket, snapshot.is_critical() ? 1 : 0);
}

static void send_dodge_event(Socket& socket, const Snapshot& snapshot) {
    send_string(socket, snapshot.get_attacker());
    send_string(socket, snapshot.get_target());
}

static void send_death_event(Socket& socket, const Snapshot& snapshot) {
    send_string(socket, snapshot.get_target());
}

static void send_item_event(Socket& socket, const Snapshot& snapshot) {
    send_u8(socket, snapshot.get_item_action());
    send_string(socket, snapshot.get_nick());
    send_string(socket, snapshot.get_item_name());
    send_u16(socket, snapshot.get_mapa_id());
    send_u16(socket, snapshot.get_x());
    send_u16(socket, snapshot.get_y());
    send_u16(socket, snapshot.get_amount());
}

static void send_chat_message(Socket& socket, const Snapshot& snapshot) {
    send_string(socket, snapshot.get_nick());
    send_string(socket, snapshot.get_target());
    send_string(socket, snapshot.get_text());
}

static void send_player_stats(Socket& socket, const Snapshot& snapshot) {
    send_string(socket, snapshot.get_nick());
    send_string(socket, snapshot.get_raza());
    send_string(socket, snapshot.get_clase());
    send_u16(socket, snapshot.get_mapa_id());
    send_u16(socket, snapshot.get_x());
    send_u16(socket, snapshot.get_y());
    send_u8(socket, snapshot.get_direction());
    send_u16(socket, snapshot.get_nivel());
    send_u16(socket, snapshot.get_vida());
    send_u16(socket, snapshot.get_vida_max());
    send_u16(socket, snapshot.get_mana());
    send_u16(socket, snapshot.get_mana_max());
    send_u32(socket, snapshot.get_experiencia());
    send_u32(socket, snapshot.get_oro());
    send_u16(socket, snapshot.get_constitucion());
    send_u16(socket, snapshot.get_inteligencia());
    send_u16(socket, snapshot.get_fuerza());
    send_u16(socket, snapshot.get_agilidad());
}

static void send_inventory_update(Socket& socket, const Snapshot& snapshot) {
    send_string(socket, snapshot.get_nick());

    const auto& items = snapshot.get_inventory_items();

    send_u16(socket, static_cast<uint16_t>(items.size()));

    for (const auto& item : items) {
        send_u16(socket, item.slot_id);
        send_string(socket, item.item);
        send_u16(socket, item.cantidad);
        send_u8(socket, item.equipado ? 1 : 0);
    }
}

static void send_error_message(Socket& socket, const Snapshot& snapshot) {
    send_string(socket, snapshot.get_nick());
    send_string(socket, snapshot.get_text());
}

static void send_meditation_status(Socket& socket, const Snapshot& snapshot) {
    send_string(socket, snapshot.get_nick());
    send_u8(socket, snapshot.is_meditating() ? 1 : 0);
}

static void send_cheat_status(Socket& socket, const Snapshot& snapshot) {
    send_string(socket, snapshot.get_nick());
    send_u8(socket, snapshot.get_cheat_type());
    send_u8(socket, snapshot.is_cheat_enabled() ? 1 : 0);
}
static void send_map_change(Socket& socket, const Snapshot& snapshot) {
    send_string(socket, snapshot.get_nick());
    send_u16(socket, snapshot.get_mapa_id());
    send_u16(socket, snapshot.get_x());
    send_u16(socket, snapshot.get_y());
    send_u8(socket, snapshot.get_direction());
}

void send_snapshot_payload(Socket& socket, const Snapshot& snapshot) {
    switch (snapshot.get_opcode()) {
        case protocol::ServerOpcode::ENTITY_CREATED:
        case protocol::ServerOpcode::ENTITY_LOGIN:
        case protocol::ServerOpcode::ENTITY_MOVE:
            send_position_snapshot(socket, snapshot);
            return;

        case protocol::ServerOpcode::ENTITY_REMOVE:
            send_entity_remove(socket, snapshot);
            return;

        case protocol::ServerOpcode::DAMAGE_EVENT:
            send_damage_event(socket, snapshot);
            return;
        case protocol::ServerOpcode::ITEM_EVENT:
            send_item_event(socket, snapshot);
            return;
        case protocol::ServerOpcode::DODGE_EVENT:
            send_dodge_event(socket, snapshot);
            return;

        case protocol::ServerOpcode::DEATH_EVENT:
            send_death_event(socket, snapshot);
            return;

        case protocol::ServerOpcode::CHAT_MESSAGE:
            send_chat_message(socket, snapshot);
            return;

        case protocol::ServerOpcode::PLAYER_STATS:
            send_player_stats(socket, snapshot);
            return;
        case protocol::ServerOpcode::INVENTORY_UPDATE:
            send_inventory_update(socket, snapshot);
            return;
        case protocol::ServerOpcode::MEDITATION_STATUS:
            send_meditation_status(socket, snapshot);
            return;
        case protocol::ServerOpcode::CHEAT_STATUS:
            send_cheat_status(socket, snapshot);
            return;
        case protocol::ServerOpcode::MAP_CHANGE:
            send_map_change(socket, snapshot);
            return;
        case protocol::ServerOpcode::ERROR_MESSAGE:
            send_error_message(socket, snapshot);
            return;

        default:
            throw std::runtime_error("Snapshot::send opcode no soportado");
    }
}

static Snapshot recv_position_snapshot(Socket& socket, uint16_t payload_size,
                                       protocol::ServerOpcode opcode) {
    std::string nick = recv_string(socket);
    uint16_t mapa_id = recv_u16(socket);
    uint16_t x = recv_u16(socket);
    uint16_t y = recv_u16(socket);
    uint8_t direction = recv_u8(socket);

    validate_payload_size(
        payload_size,
        static_cast<uint16_t>(sizeof(uint16_t) + nick.size() +
                              sizeof(uint16_t) + sizeof(uint16_t) +
                              sizeof(uint16_t) + sizeof(uint8_t)),
        "Snapshot::recv payload_size invalido");

    if (opcode == protocol::ServerOpcode::ENTITY_CREATED) {
        return Snapshot::entity_created(nick, mapa_id, x, y, direction);
    }

    if (opcode == protocol::ServerOpcode::ENTITY_LOGIN) {
        return Snapshot::entity_login(nick, mapa_id, x, y, direction);
    }

    return Snapshot::entity_move(nick, mapa_id, x, y, direction);
}

static Snapshot recv_entity_remove(Socket& socket, uint16_t payload_size) {
    std::string nick = recv_string(socket);
    validate_payload_size(payload_size,
                          static_cast<uint16_t>(sizeof(uint16_t) + nick.size()),
                          "Snapshot::recv payload_size invalido");
    return Snapshot::entity_remove(nick);
}

static Snapshot recv_damage_event(Socket& socket, uint16_t payload_size) {
    std::string attacker = recv_string(socket);
    std::string target = recv_string(socket);
    uint16_t damage = recv_u16(socket);
    uint8_t critical = recv_u8(socket);
    validate_payload_size(
        payload_size,
        static_cast<uint16_t>(sizeof(uint16_t) + attacker.size() +
                              sizeof(uint16_t) + target.size() +
                              sizeof(uint16_t) + sizeof(uint8_t)),
        "Snapshot::recv payload_size invalido");
    return Snapshot::damage_event(attacker, target, damage, critical != 0);
}

static Snapshot recv_dodge_event(Socket& socket, uint16_t payload_size) {
    std::string attacker = recv_string(socket);
    std::string target = recv_string(socket);
    validate_payload_size(
        payload_size,
        static_cast<uint16_t>(sizeof(uint16_t) + attacker.size() +
                              sizeof(uint16_t) + target.size()),
        "Snapshot::recv payload_size invalido");
    return Snapshot::dodge_event(attacker, target);
}

static Snapshot recv_death_event(Socket& socket, uint16_t payload_size) {
    std::string target = recv_string(socket);
    validate_payload_size(
        payload_size, static_cast<uint16_t>(sizeof(uint16_t) + target.size()),
        "Snapshot::recv payload_size invalido");
    return Snapshot::death_event(target);
}

static Snapshot recv_item_event(Socket& socket, uint16_t payload_size) {
    uint8_t action = recv_u8(socket);
    std::string entity_name = recv_string(socket);
    std::string item_name = recv_string(socket);
    uint16_t mapa_id = recv_u16(socket);
    uint16_t x = recv_u16(socket);
    uint16_t y = recv_u16(socket);
    uint16_t amount = recv_u16(socket);

    validate_payload_size(
        payload_size,
        static_cast<uint16_t>(
            sizeof(uint8_t) + sizeof(uint16_t) + entity_name.size() +
            sizeof(uint16_t) + item_name.size() + sizeof(uint16_t) +
            sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t)),
        "Snapshot::recv payload_size invalido ITEM_EVENT");

    return Snapshot::item_event(action, entity_name, item_name, mapa_id, x, y,
                                amount);
}

static Snapshot recv_chat_message(Socket& socket, uint16_t payload_size) {
    std::string from = recv_string(socket);
    std::string to = recv_string(socket);
    std::string text = recv_string(socket);
    validate_payload_size(payload_size,
                          static_cast<uint16_t>(sizeof(uint16_t) + from.size() +
                                                sizeof(uint16_t) + to.size() +
                                                sizeof(uint16_t) + text.size()),
                          "Snapshot::recv payload_size invalido");
    return Snapshot::chat_message(from, to, text);
}

static Snapshot recv_player_stats(Socket& socket, uint16_t payload_size) {
    std::string nick = recv_string(socket);
    std::string raza = recv_string(socket);
    std::string clase = recv_string(socket);
    uint16_t mapa_id = recv_u16(socket);
    uint16_t x = recv_u16(socket);
    uint16_t y = recv_u16(socket);
    uint8_t direction = recv_u8(socket);
    uint16_t nivel = recv_u16(socket);
    uint16_t vida = recv_u16(socket);
    uint16_t vida_max = recv_u16(socket);
    uint16_t mana = recv_u16(socket);
    uint16_t mana_max = recv_u16(socket);
    uint32_t experiencia = recv_u32(socket);
    uint32_t oro = recv_u32(socket);
    uint16_t constitucion = recv_u16(socket);
    uint16_t inteligencia = recv_u16(socket);
    uint16_t fuerza = recv_u16(socket);
    uint16_t agilidad = recv_u16(socket);
    validate_payload_size(
        payload_size,
        static_cast<uint16_t>(
            sizeof(uint16_t) + nick.size() + sizeof(uint16_t) + raza.size() +
            sizeof(uint16_t) + clase.size() + sizeof(uint16_t) +
            sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint8_t) +
            sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t) +
            sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint32_t) +
            sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint16_t) +
            sizeof(uint16_t) + sizeof(uint16_t)),
        "Snapshot::recv payload_size invalido");
    return Snapshot::player_stats(nick, raza, clase, mapa_id, x, y, direction,
                                  nivel, vida, vida_max, mana, mana_max,
                                  experiencia, oro, constitucion, inteligencia,
                                  fuerza, agilidad);
}

static Snapshot recv_inventory_update(Socket& socket, uint16_t payload_size) {
    std::string nick = recv_string(socket);

    uint16_t slots_count = recv_u16(socket);

    std::vector<InventorySnapshotItem> items;

    uint16_t expected_payload_size = static_cast<uint16_t>(
        sizeof(uint16_t) + nick.size() + sizeof(uint16_t));

    for (uint16_t i = 0; i < slots_count; ++i) {
        InventorySnapshotItem item;

        item.slot_id = recv_u16(socket);
        item.item = recv_string(socket);
        item.cantidad = recv_u16(socket);
        item.equipado = recv_u8(socket) != 0;

        expected_payload_size = static_cast<uint16_t>(
            expected_payload_size + sizeof(uint16_t) + sizeof(uint16_t) +
            item.item.size() + sizeof(uint16_t) + sizeof(uint8_t));

        items.push_back(item);
    }

    validate_payload_size(
        payload_size, expected_payload_size,
        "Snapshot::recv payload_size invalido INVENTORY_UPDATE");

    return Snapshot::inventory_update(nick, items);
}

static Snapshot recv_error_message(Socket& socket, uint16_t payload_size) {
    std::string nick = recv_string(socket);
    std::string text = recv_string(socket);
    validate_payload_size(payload_size,
                          static_cast<uint16_t>(sizeof(uint16_t) + nick.size() +
                                                sizeof(uint16_t) + text.size()),
                          "Snapshot::recv payload_size invalido");
    return Snapshot::error_message(nick, text);
}

static Snapshot recv_meditation_status(Socket& socket, uint16_t payload_size) {
    std::string nick = recv_string(socket);
    uint8_t started = recv_u8(socket);
    validate_payload_size(
        payload_size,
        static_cast<uint16_t>(sizeof(uint16_t) + nick.size() + sizeof(uint8_t)),
        "Snapshot::recv payload_size invalido");
    return Snapshot::meditation_status(nick, started != 0);
}

static Snapshot recv_cheat_status(Socket& socket, uint16_t payload_size) {
    std::string nick = recv_string(socket);
    uint8_t cheat_type = recv_u8(socket);
    uint8_t enabled = recv_u8(socket);

    validate_payload_size(
        payload_size,
        static_cast<uint16_t>(sizeof(uint16_t) + nick.size() + sizeof(uint8_t) +
                              sizeof(uint8_t)),
        "Snapshot::recv payload_size invalido");

    return Snapshot::cheat_status(nick, cheat_type, enabled != 0);
}

static Snapshot recv_map_change(Socket& socket, uint16_t payload_size) {
    std::string nick = recv_string(socket);
    uint16_t mapa_id = recv_u16(socket);
    uint16_t x = recv_u16(socket);
    uint16_t y = recv_u16(socket);
    uint8_t direction = recv_u8(socket);

    validate_payload_size(
        payload_size,
        static_cast<uint16_t>(sizeof(uint16_t) + nick.size() +
                              sizeof(uint16_t) + sizeof(uint16_t) +
                              sizeof(uint16_t) + sizeof(uint8_t)),
        "Snapshot::recv payload_size invalido MAP_CHANGE");

    return Snapshot::map_change(nick, mapa_id, x, y, direction);
}

Snapshot recv_snapshot_payload(Socket& socket, protocol::ServerOpcode opcode,
                               uint16_t payload_size) {
    switch (opcode) {
        case protocol::ServerOpcode::ENTITY_CREATED:
        case protocol::ServerOpcode::ENTITY_LOGIN:
        case protocol::ServerOpcode::ENTITY_MOVE:
            return recv_position_snapshot(socket, payload_size, opcode);

        case protocol::ServerOpcode::ENTITY_REMOVE:
            return recv_entity_remove(socket, payload_size);

        case protocol::ServerOpcode::DAMAGE_EVENT:
            return recv_damage_event(socket, payload_size);

        case protocol::ServerOpcode::DODGE_EVENT:
            return recv_dodge_event(socket, payload_size);

        case protocol::ServerOpcode::DEATH_EVENT:
            return recv_death_event(socket, payload_size);
        case protocol::ServerOpcode::ITEM_EVENT:
            return recv_item_event(socket, payload_size);
        case protocol::ServerOpcode::CHAT_MESSAGE:
            return recv_chat_message(socket, payload_size);

        case protocol::ServerOpcode::PLAYER_STATS:
            return recv_player_stats(socket, payload_size);
        case protocol::ServerOpcode::INVENTORY_UPDATE:
            return recv_inventory_update(socket, payload_size);
        case protocol::ServerOpcode::MEDITATION_STATUS:
            return recv_meditation_status(socket, payload_size);

        case protocol::ServerOpcode::CHEAT_STATUS:
            return recv_cheat_status(socket, payload_size);

        case protocol::ServerOpcode::MAP_CHANGE:
            return recv_map_change(socket, payload_size);

        case protocol::ServerOpcode::ERROR_MESSAGE:
            return recv_error_message(socket, payload_size);

        default:
            throw std::runtime_error("Snapshot::recv opcode no soportado");
    }
}
