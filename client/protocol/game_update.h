#ifndef CLIENT_GAME_UPDATE_H
#define CLIENT_GAME_UPDATE_H

#include <cstdint>
#include <string>
#include <vector>

#include "protocol/protocol_defs.h"

struct PlayerView {
    uint32_t id = 0;
    int16_t x = 0;
    int16_t y = 0;
    protocol::Direction direction = protocol::Direction::SOUTH;
};

struct GameUpdate {
    std::vector<PlayerView> players;
    std::string raw;
    bool disconnect = false;

    bool empty() const { return players.empty() && raw.empty(); }
};

#endif
