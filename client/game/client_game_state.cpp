#include "game/client_game_state.h"

#include "common/snapshot.h"

ClientGameState::ClientGameState(const std::string& local_nick,
                                 int map_width, int map_height):
        local_nick(local_nick),
        has_local_pos(false),
        local_x(0),
        local_y(0),
        local_dir(protocol::Direction::SOUTH),
        local_moved(false),
        map_width(map_width),
        map_height(map_height) {}

void ClientGameState::begin_frame() {
    local_moved = false;
    for (auto& [nick, pv] : others) {
        pv.moved = false;
    }
}

void ClientGameState::apply_update(const GameUpdate& update) {
    if (update.disconnect) {
        return;
    }
    if (update.snapshot.has_value()) {
        apply_snapshot(*update.snapshot);
    }
}

void ClientGameState::apply_snapshot(const Snapshot& snapshot) {
    if (snapshot.is_entity_move()) {
        apply_entity_move(snapshot);
    } else if (snapshot.is_entity_remove()) {
        apply_entity_remove(snapshot);
    }
}

void ClientGameState::apply_entity_move(const Snapshot& snapshot) {
    const std::string& nick = snapshot.get_nick();

    if (nick == local_nick) {
        const uint16_t new_x = snapshot.get_x();
        const uint16_t new_y = snapshot.get_y();
        local_moved = has_local_pos && (new_x != local_x || new_y != local_y);
        local_x = new_x;
        local_y = new_y;
        local_dir = static_cast<protocol::Direction>(snapshot.get_direction());
        has_local_pos = true;
        return;
    }

    PlayerView& pv = others[nick];
    const uint16_t new_x = snapshot.get_x();
    const uint16_t new_y = snapshot.get_y();
    pv.moved = (new_x != pv.x || new_y != pv.y);
    pv.nick = nick;
    pv.x = new_x;
    pv.y = new_y;
    pv.direction = static_cast<protocol::Direction>(snapshot.get_direction());
}

void ClientGameState::apply_entity_remove(const Snapshot& snapshot) {
    const std::string& nick = snapshot.get_nick();
    if (nick == local_nick) {
        return;
    }
    others.erase(nick);
}