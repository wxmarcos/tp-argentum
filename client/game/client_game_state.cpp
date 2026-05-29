#include "game/client_game_state.h"

#include <algorithm>

ClientGameState::ClientGameState(int start_x, int start_y, int map_width,
                                 int map_height):
        player_x(start_x),
        player_y(start_y),
        player_dir(protocol::Direction::SOUTH),
        map_width(map_width),
        map_height(map_height) {}

void ClientGameState::apply_update(const GameUpdate& update) {
    if (update.disconnect) {
        return;
    }
    if (!update.players.empty()) {
        apply_snapshot(update);
    }
}

void ClientGameState::apply_snapshot(const GameUpdate& update) {
    const PlayerView& me = update.players.front();
    player_x = me.x;
    player_y = me.y;
    player_dir = me.direction;
}