#ifndef CLIENT_CLIENT_GAME_STATE_H
#define CLIENT_CLIENT_GAME_STATE_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/protocol_defs.h"
#include "protocol/game_update.h"

struct PlayerStats {
    std::string raza;
    std::string clase;
    uint16_t nivel = 0;
    uint16_t vida = 0;
    uint16_t vida_max = 0;
    uint16_t mana = 0;
    uint16_t mana_max = 0;
    uint32_t experiencia = 0;
    uint32_t oro = 0;
    uint16_t constitucion = 0;
    uint16_t inteligencia = 0;
    uint16_t fuerza = 0;
    uint16_t agilidad = 0;
};

struct PlayerView {
    std::string nick;
    std::string raza;
    std::string clase;
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

struct InventorySlotView {
    std::string item;
    uint16_t cantidad = 0;
    bool equipado = false;
    bool empty() const { return item.empty(); }
};

enum class FloatingKind { DamageDealt, DamageReceived, Crit, Dodge, Death };

struct FloatingEvent {
    uint16_t x = 0;
    uint16_t y = 0;
    std::string text;
    FloatingKind kind = FloatingKind::DamageDealt;
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

    PlayerStats local_stats;
    bool has_stats;

    std::string last_error;
    bool has_error;
    uint32_t error_seq;

    std::unordered_map<std::string, PlayerView> others;
    std::unordered_map<std::string, CreatureView> creatures;

    int map_width;
    int map_height;

    std::vector<FloatingEvent> floating_events;

    std::vector<InventorySlotView> inventory;
    bool inventory_open = false;

    bool resolve_entity_pos(const std::string& nick, uint16_t& x,
                            uint16_t& y) const;

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

    bool has_local_stats() const { return has_stats; }
    const PlayerStats& get_local_stats() const { return local_stats; }
    uint16_t get_current_map_id() const { return current_map_id; }

    bool has_pending_error() const { return has_error; }
    const std::string& get_last_error() const { return last_error; }
    uint32_t get_error_seq() const { return error_seq; }

    const std::vector<InventorySlotView>& get_inventory() const { return inventory; }
    bool is_inventory_open() const { return inventory_open; }
    void toggle_inventory() { inventory_open = !inventory_open; }

    const std::unordered_map<std::string, PlayerView>& get_others() const {
        return others;
    }

    const std::vector<FloatingEvent>& get_floating_events() const {
        return floating_events;
    }

    const std::unordered_map<std::string, CreatureView>& get_creatures() const {
        return creatures;
    }

    bool entity_at(uint16_t x, uint16_t y, std::string& out_nick) const;

    int get_map_width() const { return map_width; }
    int get_map_height() const { return map_height; }
};

#endif