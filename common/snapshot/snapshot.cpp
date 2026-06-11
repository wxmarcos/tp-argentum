#include "common/snapshot/snapshot.h"

#include <stdexcept>
#include <string>

#include "common/protocol_defs.h"
#include "common/snapshot/snapshot_codec.h"
#include "network/socket.h"
#include "network/socket_helpers.h"

Snapshot::Snapshot(protocol::ServerOpcode opcode, const std::string& nick,
                   uint16_t x, uint16_t y, uint8_t direction):
    opcode(opcode),
    nick(nick), x(x), y(y), direction(direction) {}

Snapshot Snapshot::entity_created(const std::string& nick, uint16_t x,
                                  uint16_t y, uint8_t direction) {
    return Snapshot(protocol::ServerOpcode::ENTITY_CREATED, nick, x, y,
                    direction);
}

Snapshot Snapshot::entity_login(const std::string& nick, uint16_t x, uint16_t y,
                                uint8_t direction) {
    return Snapshot(protocol::ServerOpcode::ENTITY_LOGIN, nick, x, y,
                    direction);
}

Snapshot Snapshot::entity_move(const std::string& nick, uint16_t x, uint16_t y,
                               uint8_t direction) {
    return Snapshot(protocol::ServerOpcode::ENTITY_MOVE, nick, x, y, direction);
}

Snapshot Snapshot::entity_remove(const std::string& nick) {
    return Snapshot(protocol::ServerOpcode::ENTITY_REMOVE, nick);
}

Snapshot Snapshot::damage_event(const std::string& attacker,
                                const std::string& target, uint16_t damage,
                                bool critical) {
    Snapshot snapshot(protocol::ServerOpcode::DAMAGE_EVENT, "");

    snapshot.attacker = attacker;
    snapshot.target = target;
    snapshot.damage = damage;
    snapshot.critical = critical;

    return snapshot;
}

Snapshot Snapshot::dodge_event(const std::string& attacker,
                               const std::string& target) {
    Snapshot snapshot(protocol::ServerOpcode::DODGE_EVENT, "");

    snapshot.attacker = attacker;
    snapshot.target = target;

    return snapshot;
}

Snapshot Snapshot::death_event(const std::string& target) {
    Snapshot snapshot(protocol::ServerOpcode::DEATH_EVENT, target);
    snapshot.target = target;
    return snapshot;
}

void Snapshot::send(Socket& socket) const {
    uint8_t raw_opcode = static_cast<uint8_t>(opcode);
    uint16_t payload_size = snapshot_payload_size(*this);

    send_u8(socket, raw_opcode);
    send_u16(socket, payload_size);
    send_snapshot_payload(socket, *this);
}

Snapshot Snapshot::recv(Socket& socket) {
    uint8_t raw_opcode = recv_u8(socket);
    uint16_t payload_size = recv_u16(socket);

    auto opcode = static_cast<protocol::ServerOpcode>(raw_opcode);
    return recv_snapshot_payload(socket, opcode, payload_size);
}

protocol::ServerOpcode Snapshot::get_opcode() const { return opcode; }
Snapshot Snapshot::error_message(const std::string& nick,
                                 const std::string& text) {
    Snapshot snapshot(protocol::ServerOpcode::ERROR_MESSAGE, nick);
    snapshot.text = text;
    return snapshot;
}

bool Snapshot::is_error_message() const {
    return opcode == protocol::ServerOpcode::ERROR_MESSAGE;
}

Snapshot Snapshot::chat_message(const std::string& from, const std::string& to,
                                const std::string& text) {
    Snapshot snapshot(protocol::ServerOpcode::CHAT_MESSAGE, from);
    snapshot.target = to;
    snapshot.text = text;
    return snapshot;
}

Snapshot Snapshot::meditation_status(const std::string& nick, bool started) {
    Snapshot snapshot(protocol::ServerOpcode::MEDITATION_STATUS, nick);
    snapshot.meditating = started;
    return snapshot;
}
Snapshot Snapshot::cheat_status(const std::string& nick,uint8_t cheat_type,bool enabled) {

    Snapshot snapshot(
        protocol::ServerOpcode::CHEAT_STATUS,
        nick);

    snapshot.cheat_type = cheat_type;
    snapshot.cheat_enabled = enabled;

    return snapshot;
}

Snapshot Snapshot::player_stats(
    const std::string& nick, const std::string& raza, const std::string& clase,
    uint16_t mapa_id, uint16_t x, uint16_t y, uint8_t direction, uint16_t nivel,
    uint16_t vida, uint16_t vida_max, uint16_t mana, uint16_t mana_max,
    uint32_t experiencia, uint32_t oro, uint16_t constitucion,
    uint16_t inteligencia, uint16_t fuerza, uint16_t agilidad) {
    Snapshot snapshot(protocol::ServerOpcode::PLAYER_STATS, nick);

    snapshot.raza = raza;
    snapshot.clase = clase;
    snapshot.mapa_id = mapa_id;
    snapshot.x = x;
    snapshot.y = y;
    snapshot.direction = direction;
    snapshot.nivel = nivel;
    snapshot.vida = vida;
    snapshot.vida_max = vida_max;
    snapshot.mana = mana;
    snapshot.mana_max = mana_max;
    snapshot.experiencia = experiencia;
    snapshot.oro = oro;
    snapshot.constitucion = constitucion;
    snapshot.inteligencia = inteligencia;
    snapshot.fuerza = fuerza;
    snapshot.agilidad = agilidad;

    return snapshot;
}
Snapshot Snapshot::inventory_update(
    const std::string& nick, const std::vector<InventorySnapshotItem>& items) {
    Snapshot snapshot(protocol::ServerOpcode::INVENTORY_UPDATE, nick);
    snapshot.inventory_items = items;
    return snapshot;
}
bool Snapshot::is_inventory_update() const {
    return opcode == protocol::ServerOpcode::INVENTORY_UPDATE;
}

const std::vector<InventorySnapshotItem>& Snapshot::get_inventory_items()
    const {
    return inventory_items;
}

Snapshot Snapshot::map_change(const std::string& nick, uint16_t mapa_id,
                              uint16_t x, uint16_t y, uint8_t direction) {
    Snapshot snapshot(protocol::ServerOpcode::MAP_CHANGE, nick, x, y,
                      direction);
    snapshot.mapa_id = mapa_id;
    return snapshot;
}

bool Snapshot::is_map_change() const {
    return opcode == protocol::ServerOpcode::MAP_CHANGE;
}

bool Snapshot::is_chat_message() const {
    return opcode == protocol::ServerOpcode::CHAT_MESSAGE;
}

const std::string& Snapshot::get_text() const { return text; }

bool Snapshot::is_entity_created() const {
    return opcode == protocol::ServerOpcode::ENTITY_CREATED;
}

bool Snapshot::is_entity_login() const {
    return opcode == protocol::ServerOpcode::ENTITY_LOGIN;
}

bool Snapshot::is_entity_move() const {
    return opcode == protocol::ServerOpcode::ENTITY_MOVE;
}

bool Snapshot::is_entity_remove() const {
    return opcode == protocol::ServerOpcode::ENTITY_REMOVE;
}

bool Snapshot::is_damage_event() const {
    return opcode == protocol::ServerOpcode::DAMAGE_EVENT;
}

bool Snapshot::is_dodge_event() const {
    return opcode == protocol::ServerOpcode::DODGE_EVENT;
}

bool Snapshot::is_death_event() const {
    return opcode == protocol::ServerOpcode::DEATH_EVENT;
}

const std::string& Snapshot::get_nick() const { return nick; }

const std::string& Snapshot::get_attacker() const { return attacker; }

const std::string& Snapshot::get_target() const { return target; }

uint16_t Snapshot::get_x() const { return x; }

uint16_t Snapshot::get_y() const { return y; }

uint8_t Snapshot::get_direction() const { return direction; }

uint16_t Snapshot::get_damage() const { return damage; }

bool Snapshot::is_critical() const { return critical; }

bool Snapshot::is_meditating() const { return meditating; }

bool Snapshot::is_meditation_status() const {
    return opcode == protocol::ServerOpcode::MEDITATION_STATUS;
}

bool Snapshot::is_player_stats() const {
    return opcode == protocol::ServerOpcode::PLAYER_STATS;
}

const std::string& Snapshot::get_raza() const { return raza; }

const std::string& Snapshot::get_clase() const { return clase; }

uint16_t Snapshot::get_mapa_id() const { return mapa_id; }

uint16_t Snapshot::get_nivel() const { return nivel; }

uint16_t Snapshot::get_vida() const { return vida; }

uint16_t Snapshot::get_vida_max() const { return vida_max; }

uint16_t Snapshot::get_mana() const { return mana; }

uint16_t Snapshot::get_mana_max() const { return mana_max; }

uint32_t Snapshot::get_experiencia() const { return experiencia; }

uint32_t Snapshot::get_oro() const { return oro; }

uint16_t Snapshot::get_constitucion() const { return constitucion; }

uint16_t Snapshot::get_inteligencia() const { return inteligencia; }

uint16_t Snapshot::get_fuerza() const { return fuerza; }

uint16_t Snapshot::get_agilidad() const { return agilidad; }

uint8_t Snapshot::get_cheat_type() const { return cheat_type;}

bool Snapshot::is_cheat_enabled() const {
    return cheat_enabled;
}
