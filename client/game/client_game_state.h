#ifndef CLIENT_CLIENT_GAME_STATE_H
#define CLIENT_CLIENT_GAME_STATE_H

#include <string>

#include "protocol/game_update.h"
#include "protocol/protocol_defs.h"

class ClientGameState {
    private:
    int player_x;
    int player_y;
    protocol::Direction player_dir;

    int map_width;
    int map_height;

    void apply_move_echo(const std::string& raw);

    void apply_move_confirmation(protocol::Direction dir);

    void apply_snapshot(const GameUpdate& update);

    public:
    ClientGameState(int start_x, int start_y, int map_width, int map_height);

    void apply_update(const GameUpdate& update);

    int get_player_x() const { return player_x; }
    int get_player_y() const { return player_y; }
    protocol::Direction get_player_dir() const { return player_dir; }
    int get_map_width() const { return map_width; }
    int get_map_height() const { return map_height; }
};

#endif