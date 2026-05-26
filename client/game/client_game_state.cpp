#include "game/client_game_state.h"

#include <sstream>
#include <string>

#include "protocol/protocol.h"

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
        return;
    }

    apply_move_echo(update.raw);
}

void ClientGameState::apply_move_echo(const std::string& raw) {
    std::istringstream is(raw);
    std::string token;
    while (is >> token) {
        if (token == protocol::wire::MOVE) {
            std::string dir_token;
            if (is >> dir_token) {
                apply_move_confirmation(
                    Protocol::direction_from_wire(dir_token));
            }
            return;
        }
    }
}

void ClientGameState::apply_move_confirmation(protocol::Direction dir) {
    int nx = player_x;
    int ny = player_y;

    switch (dir) {
        case protocol::Direction::NORTH:
            ny -= 1;
            break;
        case protocol::Direction::SOUTH:
            ny += 1;
            break;
        case protocol::Direction::EAST:
            nx += 1;
            break;
        case protocol::Direction::WEST:
            nx -= 1;
            break;
    }

    player_dir = dir;

    if (nx >= 0 && nx < map_width && ny >= 0 && ny < map_height) {
        player_x = nx;
        player_y = ny;
    }
}

void ClientGameState::apply_snapshot(const GameUpdate& update) {
    const PlayerView& me = update.players.front();
    player_x = me.x;
    player_y = me.y;
    player_dir = me.direction;
}
