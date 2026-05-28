#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <cstdint>
#include <string>

#include "common/protocol_defs.h"

class Socket;

class Snapshot {

private:

    protocol::ServerOpcode opcode;

    std::string nick;

    uint16_t x;
    uint16_t y;

    uint8_t direction;

public:

    Snapshot(
        protocol::ServerOpcode opcode,
        const std::string& nick,
        uint16_t x = 0,
        uint16_t y = 0,
        uint8_t direction = 0);

    static Snapshot entity_move(
        const std::string& nick,
        uint16_t x,
        uint16_t y,
        uint8_t direction);

    static Snapshot entity_remove(
        const std::string& nick);

    void send(Socket& socket) const;

    static Snapshot recv(Socket& socket);

    protocol::ServerOpcode get_opcode() const;

    bool is_entity_move() const;

    bool is_entity_remove() const;

    const std::string& get_nick() const;

    uint16_t get_x() const;

    uint16_t get_y() const;

    uint8_t get_direction() const;
};

#endif