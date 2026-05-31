#ifndef CLIENT_GAME_UPDATE_H
#define CLIENT_GAME_UPDATE_H

#include <cstdint>
#include <vector>

#include "common/protocol_defs.h"

struct PlayerView {
    uint16_t id = 0;
    uint16_t x = 0;
    uint16_t y = 0;
    protocol::Direction direction = protocol::Direction::SOUTH;
};

struct GameUpdate {
    uint32_t tick = 0;
    uint16_t local_id = 0;
    std::vector<PlayerView> players;
    bool disconnect = false;

    bool empty() const { return players.empty(); }
};

#endif