#ifndef CLIENT_COMMAND_H
#define CLIENT_COMMAND_H

#include "protocol/protocol_defs.h"

struct ClientCommand {
    protocol::CommandType type = protocol::CommandType::UNKNOWN;
    protocol::Direction direction = protocol::Direction::SOUTH;

    ClientCommand() = default;

    static ClientCommand move(protocol::Direction dir) {
        ClientCommand cmd;
        cmd.type = protocol::CommandType::MOVE;
        cmd.direction = dir;
        return cmd;
    }
};

#endif
