#include "snapshot.h"

#include "network/socket.h"
#include "common/protocol_defs.h"

#include <arpa/inet.h>
#include <stdexcept>
#include <string>

static bool is_position_snapshot(protocol::ServerOpcode opcode) {
    return opcode == protocol::ServerOpcode::ENTITY_MOVE ||
           opcode == protocol::ServerOpcode::ENTITY_CREATED ||
           opcode == protocol::ServerOpcode::ENTITY_LOGIN;
}

static void send_u8(Socket& socket, uint8_t value) {
    socket.sendall(&value, sizeof(value));
}

static void send_u16(Socket& socket, uint16_t value) {
    uint16_t net_value = htons(value);
    socket.sendall(&net_value, sizeof(net_value));
}

static uint8_t recv_u8(Socket& socket) {
    uint8_t value;
    int received = socket.recvall(&value, sizeof(value));
    if (received == 0) throw std::runtime_error("socket cerrado leyendo uint8");
    return value;
}

static uint16_t recv_u16(Socket& socket) {
    uint16_t net_value;
    int received = socket.recvall(&net_value, sizeof(net_value));
    if (received == 0) throw std::runtime_error("socket cerrado leyendo uint16");
    return ntohs(net_value);
}

static void send_string(Socket& socket, const std::string& value) {
    send_u16(socket, static_cast<uint16_t>(value.size()));
    if (!value.empty()) {
        socket.sendall(value.data(), value.size());
    }
}

static std::string recv_string(Socket& socket) {
    uint16_t len = recv_u16(socket);

    std::string value;
    value.resize(len);

    if (len > 0) {
        int received = socket.recvall(value.data(), len);
        if (received == 0) throw std::runtime_error("socket cerrado leyendo string");
    }

    return value;
}

Snapshot::Snapshot(
    protocol::ServerOpcode opcode,
    const std::string& nick,
    uint16_t x,
    uint16_t y,
    uint8_t direction):
    opcode(opcode),
    nick(nick),
    x(x),
    y(y),
    direction(direction) {}

Snapshot Snapshot::entity_created(
    const std::string& nick,
    uint16_t x,
    uint16_t y,
    uint8_t direction) {

    return Snapshot(
        protocol::ServerOpcode::ENTITY_CREATED,
        nick,
        x,
        y,
        direction);
}

Snapshot Snapshot::entity_login(
    const std::string& nick,
    uint16_t x,
    uint16_t y,
    uint8_t direction) {

    return Snapshot(
        protocol::ServerOpcode::ENTITY_LOGIN,
        nick,
        x,
        y,
        direction);
}

Snapshot Snapshot::entity_move(
    const std::string& nick,
    uint16_t x,
    uint16_t y,
    uint8_t direction) {

    return Snapshot(
        protocol::ServerOpcode::ENTITY_MOVE,
        nick,
        x,
        y,
        direction);
}

Snapshot Snapshot::entity_remove(const std::string& nick) {
    return Snapshot(protocol::ServerOpcode::ENTITY_REMOVE, nick);
}

Snapshot Snapshot::damage_event(
    const std::string& attacker,
    const std::string& target,
    uint16_t damage,
    bool critical) {

    Snapshot snapshot(protocol::ServerOpcode::DAMAGE_EVENT, "");

    snapshot.attacker = attacker;
    snapshot.target = target;
    snapshot.damage = damage;
    snapshot.critical = critical;

    return snapshot;
}

Snapshot Snapshot::dodge_event(
    const std::string& attacker,
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

    uint16_t payload_size = 0;

    if (is_position_snapshot(opcode)) {
        payload_size =
            static_cast<uint16_t>(
                sizeof(uint16_t) + nick.size() +
                sizeof(uint16_t) +
                sizeof(uint16_t) +
                sizeof(uint8_t));

    } else if (opcode == protocol::ServerOpcode::ENTITY_REMOVE) {
        payload_size =
            static_cast<uint16_t>(
                sizeof(uint16_t) + nick.size());

    } else if (opcode == protocol::ServerOpcode::DAMAGE_EVENT) {
        payload_size =
            static_cast<uint16_t>(
                sizeof(uint16_t) + attacker.size() +
                sizeof(uint16_t) + target.size() +
                sizeof(uint16_t) +
                sizeof(uint8_t));

    } else if (opcode == protocol::ServerOpcode::DODGE_EVENT) {
        payload_size =
            static_cast<uint16_t>(
                sizeof(uint16_t) + attacker.size() +
                sizeof(uint16_t) + target.size());

    } else if (opcode == protocol::ServerOpcode::DEATH_EVENT) {
        payload_size =
            static_cast<uint16_t>(
                sizeof(uint16_t) + target.size());
                
    } else if (opcode == protocol::ServerOpcode::CHAT_MESSAGE) {
    payload_size =
        static_cast<uint16_t>(
            sizeof(uint16_t) + nick.size() +
            sizeof(uint16_t) + target.size() +
            sizeof(uint16_t) + text.size());

    }else if (opcode == protocol::ServerOpcode::ERROR_MESSAGE) {
        payload_size = static_cast<uint16_t>(
            sizeof(uint16_t) + nick.size() +
            sizeof(uint16_t) + text.size()
    );

    } else {
        throw std::runtime_error("Snapshot::send opcode no soportado");
    }

    send_u8(socket, raw_opcode);
    send_u16(socket, payload_size);

    if (is_position_snapshot(opcode)) {
        send_string(socket, nick);
        send_u16(socket, x);
        send_u16(socket, y);
        send_u8(socket, direction);
        return;
    }

    if (opcode == protocol::ServerOpcode::ENTITY_REMOVE) {
        send_string(socket, nick);
        return;
    }

    if (opcode == protocol::ServerOpcode::DAMAGE_EVENT) {
        send_string(socket, attacker);
        send_string(socket, target);
        send_u16(socket, damage);
        send_u8(socket, critical ? 1 : 0);
        return;
    }

    if (opcode == protocol::ServerOpcode::DODGE_EVENT) {
        send_string(socket, attacker);
        send_string(socket, target);
        return;
    }

    if (opcode == protocol::ServerOpcode::DEATH_EVENT) {
        send_string(socket, target);
        return;
    }
    if (opcode == protocol::ServerOpcode::CHAT_MESSAGE) {
        send_string(socket, nick);
        send_string(socket, target);
        send_string(socket, text);
        return;
    }

    if (opcode == protocol::ServerOpcode::ERROR_MESSAGE) {
        send_string(socket, nick);
        send_string(socket, text);
        return;
    }
}

Snapshot Snapshot::recv(Socket& socket) {
    uint8_t raw_opcode = recv_u8(socket);
    uint16_t payload_size = recv_u16(socket);

    auto opcode = static_cast<protocol::ServerOpcode>(raw_opcode);

    if (is_position_snapshot(opcode)) {
        std::string nick = recv_string(socket);
        uint16_t x = recv_u16(socket);
        uint16_t y = recv_u16(socket);
        uint8_t direction = recv_u8(socket);

        uint16_t expected_payload_size =
            static_cast<uint16_t>(
                sizeof(uint16_t) + nick.size() +
                sizeof(uint16_t) +
                sizeof(uint16_t) +
                sizeof(uint8_t));

        if (payload_size != expected_payload_size) {
            throw std::runtime_error("Snapshot::recv payload_size invalido position snapshot");
        }

        if (opcode == protocol::ServerOpcode::ENTITY_CREATED) {
            return Snapshot::entity_created(nick, x, y, direction);
        }

        if (opcode == protocol::ServerOpcode::ENTITY_LOGIN) {
            return Snapshot::entity_login(nick, x, y, direction);
        }

        return Snapshot::entity_move(nick, x, y, direction);
    }

    if (opcode == protocol::ServerOpcode::ENTITY_REMOVE) {
        std::string nick = recv_string(socket);

        uint16_t expected_payload_size =
            static_cast<uint16_t>(
                sizeof(uint16_t) + nick.size());

        if (payload_size != expected_payload_size) {
            throw std::runtime_error("Snapshot::recv payload_size invalido ENTITY_REMOVE");
        }

        return Snapshot::entity_remove(nick);
    }

    if (opcode == protocol::ServerOpcode::DAMAGE_EVENT) {
        std::string attacker = recv_string(socket);
        std::string target = recv_string(socket);
        uint16_t damage = recv_u16(socket);
        uint8_t critical = recv_u8(socket);

        uint16_t expected_payload_size =
            static_cast<uint16_t>(
                sizeof(uint16_t) + attacker.size() +
                sizeof(uint16_t) + target.size() +
                sizeof(uint16_t) +
                sizeof(uint8_t));

        if (payload_size != expected_payload_size) {
            throw std::runtime_error("Snapshot::recv payload_size invalido DAMAGE_EVENT");
        }

        return Snapshot::damage_event(
            attacker,
            target,
            damage,
            critical != 0);
    }

    if (opcode == protocol::ServerOpcode::DODGE_EVENT) {
        std::string attacker = recv_string(socket);
        std::string target = recv_string(socket);

        uint16_t expected_payload_size =
            static_cast<uint16_t>(
                sizeof(uint16_t) + attacker.size() +
                sizeof(uint16_t) + target.size());

        if (payload_size != expected_payload_size) {
            throw std::runtime_error("Snapshot::recv payload_size invalido DODGE_EVENT");
        }

        return Snapshot::dodge_event(attacker, target);
    }

    if (opcode == protocol::ServerOpcode::DEATH_EVENT) {
        std::string target = recv_string(socket);

        uint16_t expected_payload_size =
            static_cast<uint16_t>(
                sizeof(uint16_t) + target.size());

        if (payload_size != expected_payload_size) {
            throw std::runtime_error("Snapshot::recv payload_size invalido DEATH_EVENT");
        }

        return Snapshot::death_event(target);
    }
    if (opcode == protocol::ServerOpcode::CHAT_MESSAGE) {
        std::string from = recv_string(socket);
        std::string to = recv_string(socket);
        std::string text = recv_string(socket);

        uint16_t expected_payload_size =
            static_cast<uint16_t>(
                sizeof(uint16_t) + from.size() +
                sizeof(uint16_t) + to.size() +
                sizeof(uint16_t) + text.size());

        if (payload_size != expected_payload_size) {
            throw std::runtime_error(
                "Snapshot::recv payload_size invalido CHAT_MESSAGE"
            );
        }

        return Snapshot::chat_message(from, to, text);
    }

    if (opcode == protocol::ServerOpcode::ERROR_MESSAGE) {
        std::string nick = recv_string(socket);
        std::string text = recv_string(socket);

        uint16_t expected_payload_size = static_cast<uint16_t>(
            sizeof(uint16_t) + nick.size() +
            sizeof(uint16_t) + text.size()
        );

        if (payload_size != expected_payload_size) {
            throw std::runtime_error(
                "Snapshot::recv payload_size invalido ERROR_MESSAGE"
            );
        }

        return Snapshot::error_message(nick, text);
    }

    throw std::runtime_error("Snapshot::recv opcode no soportado");
}

protocol::ServerOpcode Snapshot::get_opcode() const {
    return opcode;
}
Snapshot Snapshot::error_message(
    const std::string& nick,
    const std::string& text
) {
    Snapshot snapshot(protocol::ServerOpcode::ERROR_MESSAGE, nick);
    snapshot.text = text;
    return snapshot;
}

bool Snapshot::is_error_message() const {
    return opcode == protocol::ServerOpcode::ERROR_MESSAGE;
}

Snapshot Snapshot::chat_message(
    const std::string& from,
    const std::string& to,
    const std::string& text
) {
    Snapshot snapshot(protocol::ServerOpcode::CHAT_MESSAGE, from);
    snapshot.target = to;
    snapshot.text = text;
    return snapshot;
}

bool Snapshot::is_chat_message() const {
    return opcode == protocol::ServerOpcode::CHAT_MESSAGE;
}

const std::string& Snapshot::get_text() const {
    return text;
}

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

const std::string& Snapshot::get_nick() const {
    return nick;
}

const std::string& Snapshot::get_attacker() const {
    return attacker;
}

const std::string& Snapshot::get_target() const {
    return target;
}

uint16_t Snapshot::get_x() const {
    return x;
}

uint16_t Snapshot::get_y() const {
    return y;
}

uint8_t Snapshot::get_direction() const {
    return direction;
}

uint16_t Snapshot::get_damage() const {
    return damage;
}

bool Snapshot::is_critical() const {
    return critical;
}