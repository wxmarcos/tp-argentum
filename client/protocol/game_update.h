#ifndef CLIENT_GAME_UPDATE_H
#define CLIENT_GAME_UPDATE_H

#include <optional>

#include "common/snapshot/snapshot.h"

struct GameUpdate {
    std::optional<Snapshot> snapshot;
    bool disconnect = false;
};

#endif
