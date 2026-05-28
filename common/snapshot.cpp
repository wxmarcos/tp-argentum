#include "snapshot.h"

#include "network/socket.h"
#include "common/protocol_defs.h"

#include <arpa/inet.h>
#include <stdexcept>
#include <vector>

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

Snapshot Snapshot::entity_remove(
    const std::string& nick) {

    return Snapshot(
        protocol::ServerOpcode::ENTITY_REMOVE,
        nick);
}

void Snapshot::send(Socket& socket) const {

    uint8_t raw_opcode =
        static_cast<uint8_t>(opcode);

    uint16_t payload_size = 0;

    if (opcode == protocol::ServerOpcode::ENTITY_MOVE) {

        payload_size =
            static_cast<uint16_t>(
                sizeof(uint16_t) +
                nick.size() +
                sizeof(uint16_t) +
                sizeof(uint16_t) +
                sizeof(uint8_t));

    } else if (opcode == protocol::ServerOpcode::ENTITY_REMOVE) {

        payload_size =
            static_cast<uint16_t>(
                sizeof(uint16_t) +
                nick.size());

    } else {

        throw std::runtime_error(
            "Snapshot::send opcode no soportado");
    }

    uint16_t net_payload_size =
        htons(payload_size);

    uint16_t net_nick_len =
        htons(static_cast<uint16_t>(nick.size()));

    socket.sendall(
        &raw_opcode,
        sizeof(raw_opcode));

    socket.sendall(
        &net_payload_size,
        sizeof(net_payload_size));

    socket.sendall(
        &net_nick_len,
        sizeof(net_nick_len));

    if (!nick.empty()) {
        socket.sendall(
            nick.data(),
            nick.size());
    }

    if (opcode == protocol::ServerOpcode::ENTITY_MOVE) {

        uint16_t net_x =
            htons(x);

        uint16_t net_y =
            htons(y);

        socket.sendall(
            &net_x,
            sizeof(net_x));

        socket.sendall(
            &net_y,
            sizeof(net_y));

        socket.sendall(
            &direction,
            sizeof(direction));
    }
}

Snapshot Snapshot::recv(Socket& socket) {

    uint8_t raw_opcode;
    uint16_t net_payload_size;

    int received =
        socket.recvall(
            &raw_opcode,
            sizeof(raw_opcode));

    if (received == 0) {
        throw std::runtime_error(
            "Snapshot::recv socket cerrado leyendo opcode");
    }

    received =
        socket.recvall(
            &net_payload_size,
            sizeof(net_payload_size));

    if (received == 0) {
        throw std::runtime_error(
            "Snapshot::recv socket cerrado leyendo payload_size");
    }

    auto opcode =
        static_cast<protocol::ServerOpcode>(raw_opcode);

    uint16_t payload_size =
        ntohs(net_payload_size);

    if (opcode != protocol::ServerOpcode::ENTITY_MOVE &&
        opcode != protocol::ServerOpcode::ENTITY_REMOVE) {

        throw std::runtime_error(
            "Snapshot::recv opcode no soportado");
    }

    uint16_t net_nick_len;

    received =
        socket.recvall(
            &net_nick_len,
            sizeof(net_nick_len));

    if (received == 0) {
        throw std::runtime_error(
            "Snapshot::recv socket cerrado leyendo nick_len");
    }

    uint16_t nick_len =
        ntohs(net_nick_len);

    uint16_t expected_payload_size = 0;

    if (opcode == protocol::ServerOpcode::ENTITY_MOVE) {

        expected_payload_size =
            static_cast<uint16_t>(
                sizeof(uint16_t) +
                nick_len +
                sizeof(uint16_t) +
                sizeof(uint16_t) +
                sizeof(uint8_t));

    } else {

        expected_payload_size =
            static_cast<uint16_t>(
                sizeof(uint16_t) +
                nick_len);
    }

    if (payload_size != expected_payload_size) {
        throw std::runtime_error(
            "Snapshot::recv payload_size invalido");
    }

    std::string nick;
    nick.resize(nick_len);

    if (nick_len > 0) {

        received =
            socket.recvall(
                nick.data(),
                nick_len);

        if (received == 0) {
            throw std::runtime_error(
                "Snapshot::recv socket cerrado leyendo nick");
        }
    }

    if (opcode == protocol::ServerOpcode::ENTITY_REMOVE) {

        return Snapshot::entity_remove(nick);
    }

    uint16_t net_x;
    uint16_t net_y;
    uint8_t direction;

    received =
        socket.recvall(
            &net_x,
            sizeof(net_x));

    if (received == 0) {
        throw std::runtime_error(
            "Snapshot::recv socket cerrado leyendo x");
    }

    received =
        socket.recvall(
            &net_y,
            sizeof(net_y));

    if (received == 0) {
        throw std::runtime_error(
            "Snapshot::recv socket cerrado leyendo y");
    }

    received =
        socket.recvall(
            &direction,
            sizeof(direction));

    if (received == 0) {
        throw std::runtime_error(
            "Snapshot::recv socket cerrado leyendo direction");
    }

    return Snapshot::entity_move(
        nick,
        ntohs(net_x),
        ntohs(net_y),
        direction);
}

protocol::ServerOpcode Snapshot::get_opcode() const {
    return opcode;
}

bool Snapshot::is_entity_move() const {
    return opcode == protocol::ServerOpcode::ENTITY_MOVE;
}

bool Snapshot::is_entity_remove() const {
    return opcode == protocol::ServerOpcode::ENTITY_REMOVE;
}

const std::string& Snapshot::get_nick() const {
    return nick;
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