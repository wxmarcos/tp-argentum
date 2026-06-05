#include "game/client_game_state.h"

#include <cctype>

#include "common/snapshot/snapshot.h"

namespace {
bool classify_creature(const std::string& nick, std::string& type) {
    if (nick.size() < 2 || nick[0] != '#') {
        return false;
    }
    const auto sep = nick.find('#', 1);
    if (sep == std::string::npos || sep == 1) {
        return false;
    }
    type = nick.substr(1, sep - 1);
    for (char& c : type) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return !type.empty();
}
}  // namespace

ClientGameState::ClientGameState(const std::string& local_nick, int map_width,
                                 int map_height):
    local_nick(local_nick),
    has_local_pos(false), local_x(0), local_y(0),
    local_dir(protocol::Direction::SOUTH), local_moved(false),
    map_width(map_width), map_height(map_height) {}

void ClientGameState::begin_frame() {
    local_moved = false;
    for (auto& [nick, pv] : others) {
        pv.moved = false;
    }
    for (auto& [key, cv] : creatures) {
        cv.moved = false;
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

    std::string type;
    if (classify_creature(nick, type)) {
        CreatureView& cv = creatures[nick];
        const uint16_t new_x = snapshot.get_x();
        const uint16_t new_y = snapshot.get_y();
        cv.moved = (new_x != cv.x || new_y != cv.y);
        cv.key = nick;
        cv.type = type;
        cv.x = new_x;
        cv.y = new_y;
        cv.direction =
            static_cast<protocol::Direction>(snapshot.get_direction());
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
    std::string type;
    if (classify_creature(nick, type)) {
        creatures.erase(nick);
        return;
    }
    others.erase(nick);
}
