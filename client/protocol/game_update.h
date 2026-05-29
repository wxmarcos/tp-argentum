#ifndef CLIENT_GAME_UPDATE_H
#define CLIENT_GAME_UPDATE_H

#include <cstdint>
#include <vector>

#include "common/protocol_defs.h"

struct PlayerView {
    uint32_t id = 0;
    int16_t x = 0;
    int16_t y = 0;
    protocol::Direction direction = protocol::Direction::SOUTH;
};

struct GameUpdate {
    std::vector<PlayerView> players;
    bool disconnect = false;

    bool empty() const { return players.empty(); }
};

#endif