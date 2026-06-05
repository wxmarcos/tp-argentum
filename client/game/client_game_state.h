#ifndef CLIENT_CLIENT_GAME_STATE_H
#define CLIENT_CLIENT_GAME_STATE_H

#include <cstdint>
#include <string>
#include <unordered_map>

#include "common/protocol_defs.h"
#include "protocol/game_update.h"

struct PlayerView {
    std::string nick;
    uint16_t x = 0;
    uint16_t y = 0;
    protocol::Direction direction = protocol::Direction::SOUTH;
    bool moved = false;
};

struct CreatureView {
    std::string key;
    std::string type;
    uint16_t x = 0;
    uint16_t y = 0;
    protocol::Direction direction = protocol::Direction::SOUTH;
    bool moved = false;
};

class ClientGameState {
private:
    std::string local_nick;

    bool has_local_pos;
    uint16_t local_x;
    uint16_t local_y;
    protocol::Direction local_dir;
    bool local_moved;
    uint16_t current_map_id;
    std::unordered_map<std::string, PlayerView> others;
    std::unordered_map<std::string, CreatureView> creatures;

    int map_width;
    int map_height;

private:
    void apply_snapshot(const Snapshot& snapshot);
    void apply_entity_position(const Snapshot& snapshot);
    void apply_entity_remove(const Snapshot& snapshot);
    void apply_player_stats(const Snapshot& snapshot);
    void apply_inventory_update(const Snapshot& snapshot);
    void apply_damage_event(const Snapshot& snapshot);
    void apply_dodge_event(const Snapshot& snapshot);
    void apply_death_event(const Snapshot& snapshot);
    void apply_meditation_status(const Snapshot& snapshot);
    void apply_chat_message(const Snapshot& snapshot);
    void apply_error_message(const Snapshot& snapshot);

public:
    ClientGameState(const std::string& local_nick, int map_width,
                    int map_height);

    void begin_frame();

    void apply_update(const GameUpdate& update);

    bool has_local_position() const { return has_local_pos; }
    uint16_t get_local_x() const { return local_x; }
    uint16_t get_local_y() const { return local_y; }
    protocol::Direction get_local_dir() const { return local_dir; }
    bool get_local_moved() const { return local_moved; }
    const std::string& get_local_nick() const { return local_nick; }

    const std::unordered_map<std::string, PlayerView>& get_others() const {
        return others;
    }

    const std::unordered_map<std::string, CreatureView>& get_creatures() const {
        return creatures;
    }

    int get_map_width() const { return map_width; }
    int get_map_height() const { return map_height; }
};

#endif
