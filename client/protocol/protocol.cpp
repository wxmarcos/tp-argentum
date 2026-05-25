#include "protocol/protocol.h"

#include <sstream>
#include <string>
#include <vector>

std::string Protocol::direction_to_wire(protocol::Direction dir) {
    switch (dir) {
        case protocol::Direction::NORTH: return protocol::wire::DIR_NORTH;
        case protocol::Direction::EAST:  return protocol::wire::DIR_EAST;
        case protocol::Direction::SOUTH: return protocol::wire::DIR_SOUTH;
        case protocol::Direction::WEST:  return protocol::wire::DIR_WEST;
    }
    throw std::runtime_error("Direccion invalida");
}

protocol::Direction Protocol::direction_from_wire(const std::string& token) {
    if (token == protocol::wire::DIR_NORTH)
        return protocol::Direction::NORTH;
    if (token == protocol::wire::DIR_EAST)
        return protocol::Direction::EAST;
    if (token == protocol::wire::DIR_WEST)
        return protocol::Direction::WEST;
    return protocol::Direction::SOUTH;
}

std::string Protocol::serialize(const ClientCommand& cmd) {
    std::ostringstream os;

    switch (cmd.type) {
        case protocol::CommandType::MOVE:
            os << protocol::wire::MOVE << protocol::wire::FIELD_SEP
               << direction_to_wire(cmd.direction);
            break;

        case protocol::CommandType::UNKNOWN:
        default:
            return std::string();
    }

    os << protocol::wire::MSG_END;
    return os.str();
}

GameUpdate Protocol::parse(const std::string& message) {
    GameUpdate update;
    update.raw = message;

    if (message == "__DISCONNECT__") {
        update.disconnect = true;
        return update;
    }

    std::istringstream is(message);
    std::string token;
    while (is >> token) {
        if (token == "PLAYER") {
            PlayerView pv;
            std::string dir_token;
            if (is >> pv.id >> pv.x >> pv.y >> dir_token) {
                pv.direction = direction_from_wire(dir_token);
                update.players.push_back(pv);
            }
        }
    }

    return update;
}
