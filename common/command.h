#pragma once

#include <cstdint>
#include <arpa/inet.h>

class Socket;

enum class CommandType : uint8_t {
    Move = 1,
    Attack = 2,
    Disconnect = 255
};

class Command {
private:
    uint16_t player_id;
    CommandType type;

    // x reutilizado:
    // MOVE -> direction
    // ATTACK -> target_id
    uint16_t x;

public:
    Command(uint16_t player_id,
            CommandType type,
            uint16_t x = 0)
        : player_id(player_id),
          type(type),
          x(x) {}

    static Command recv(Socket& socket);
    void send(Socket& socket) const;

    bool is_disconnect() const {
        return type == CommandType::Disconnect;
    }

    CommandType get_type() const {
        return type;
    }

    uint16_t get_player_id() const {
        return player_id;
    }

    uint16_t get_x() const {
        return x;
    }

    uint16_t get_direction() const {
        return x; // MOVE
    }

    uint16_t get_target() const {
        return x; // ATTACK
    }
};