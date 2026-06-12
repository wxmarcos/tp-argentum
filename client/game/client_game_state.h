#ifndef CLIENT_CLIENT_GAME_STATE_H
#define CLIENT_CLIENT_GAME_STATE_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "common/protocol_defs.h"
#include "protocol/game_update.h"
#include "game/floating_kind.h"
#include "render/effects/effect_spawn.h"

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
    bool empty() const;
};

struct ChatMessage {
    std::string from;
    std::string text;
};

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
    std::unordered_set<std::string> dead_entities;

    int map_width;
    int map_height;

    std::vector<FloatingEvent> floating_events;
    std::vector<EffectSpawn> effect_spawns;

    std::vector<InventorySlotView> inventory;
    bool inventory_open = false;

    std::vector<ChatMessage> chat_messages;

    bool resolve_entity_pos(const std::string& nick, uint16_t& x,
                            uint16_t& y) const;

    bool classify_creature(const std::string& nick, std::string& type) const;
    std::string to_lower(std::string s) const;

    void apply_snapshot(const Snapshot& snapshot);
    void apply_entity_position(const Snapshot& snapshot);
    void apply_local_position(const Snapshot& snapshot);
    void apply_creature_position(const Snapshot& snapshot,
                                 const std::string& type);
    void apply_other_player_position(const Snapshot& snapshot);
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

    bool has_local_position() const;
    uint16_t get_local_x() const;
    uint16_t get_local_y() const;
    protocol::Direction get_local_dir() const;
    bool get_local_moved() const;
    const std::string& get_local_nick() const;

    bool has_local_stats() const;
    const PlayerStats& get_local_stats() const;
    uint16_t get_current_map_id() const;

    bool has_pending_error() const;
    const std::string& get_last_error() const;
    uint32_t get_error_seq() const;

    const std::vector<InventorySlotView>& get_inventory() const;
    bool is_inventory_open() const;
    void toggle_inventory();

    const std::unordered_map<std::string, PlayerView>& get_others() const;
    const std::vector<FloatingEvent>& get_floating_events() const;
    const std::vector<EffectSpawn>& get_effect_spawns() const;
    const std::unordered_map<std::string, CreatureView>& get_creatures() const;

    bool is_dead(const std::string& nick) const;
    bool entity_at(uint16_t x, uint16_t y, std::string& out_nick) const;

    int get_map_width() const;
    int get_map_height() const;

    const std::vector<ChatMessage>& get_chat_messages() const;
};

#endif