#include "game/client_game_state.h"

#include <cctype>
#include <cstddef>
#include <string_view>
#include <unordered_set>

#include "common/snapshot/snapshot.h"
#include "game/entity_keys.h"

static constexpr size_t MAX_CHAT_MESSAGES = 50;

bool InventorySlotView::empty() const { return item.empty(); }

ClientGameState::ClientGameState(const std::string& local_nick, int map_width,
                                 int map_height):
    local_nick(local_nick),
    has_local_pos(false), local_x(0), local_y(0),
    local_dir(protocol::Direction::SOUTH), local_moved(false),
    current_map_id(0), has_stats(false), has_error(false), error_seq(0),
    map_width(map_width), map_height(map_height) {}

bool ClientGameState::classify_creature(const std::string& nick,
                                        std::string& type) const {
    static const std::unordered_set<std::string_view> KNOWN_TYPES = {
        keys::GOBLIN, keys::ESQUELETO, keys::ZOMBIE, keys::ARANA,
        keys::ORCO, keys::GOLEM, keys::BANQUERO, keys::COMERCIANTE,
        keys::SACERDOTE};

    const auto sep = nick.rfind('_');
    if (sep == std::string::npos || sep == 0 || sep + 1 >= nick.size()) {
        return false;
    }
    for (size_t i = sep + 1; i < nick.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(nick[i]))) {
            return false;
        }
    }
    std::string prefix = to_lower(nick.substr(0, sep));
    if (KNOWN_TYPES.find(prefix) == KNOWN_TYPES.end()) {
        return false;
    }
    type = prefix;
    return true;
}

std::string ClientGameState::to_lower(std::string s) const {
    for (char& c : s) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return s;
}

void ClientGameState::begin_frame() {
    floating_events.clear();
    effect_spawns.clear();
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
    if (snapshot.is_entity_created() || snapshot.is_entity_login() ||
        snapshot.is_entity_move() || snapshot.is_map_change()) {
        apply_entity_position(snapshot);
    } else if (snapshot.is_entity_remove()) {
        apply_entity_remove(snapshot);
    } else if (snapshot.is_player_stats()) {
        apply_player_stats(snapshot);
    } else if (snapshot.is_inventory_update()) {
        apply_inventory_update(snapshot);
    } else if (snapshot.is_damage_event()) {
        apply_damage_event(snapshot);
    } else if (snapshot.is_dodge_event()) {
        apply_dodge_event(snapshot);
    } else if (snapshot.is_death_event()) {
        apply_death_event(snapshot);
    } else if (snapshot.is_meditation_status()) {
        apply_meditation_status(snapshot);
    } else if (snapshot.is_chat_message()) {
        apply_chat_message(snapshot);
    } else if (snapshot.is_error_message()) {
        apply_error_message(snapshot);
    } else if (snapshot.is_item_event()) {
        apply_item_event(snapshot);
    } else if (snapshot.is_chat_message()) {
        apply_chat_message(snapshot);
    }
}

void ClientGameState::apply_entity_position(const Snapshot& snapshot) {
    const std::string& nick = snapshot.get_nick();
    if (nick == local_nick) {
        if (snapshot.is_entity_created()) {
            return;
        }

        if (snapshot.is_map_change()) {
            current_map_id = snapshot.get_mapa_id();
            others.clear();
            creatures.clear();
            local_moved = false;
        }

        const uint16_t new_x = snapshot.get_x();
        const uint16_t new_y = snapshot.get_y();

        local_moved = has_local_pos && !snapshot.is_map_change() &&
                    (new_x != local_x || new_y != local_y);

        local_x = new_x;
        local_y = new_y;
        local_dir = static_cast<protocol::Direction>(snapshot.get_direction());
        has_local_pos = true;

        return;
    }

    if (snapshot.get_mapa_id() != current_map_id) {
        return;
    }
    std::string type;
    if (classify_creature(nick, type)) {
        apply_creature_position(snapshot, type);
        return;
    }
    apply_other_player_position(snapshot);
}

void ClientGameState::apply_local_position(const Snapshot& snapshot) {
    const uint16_t new_x = snapshot.get_x();
    const uint16_t new_y = snapshot.get_y();
    local_moved = has_local_pos && (new_x != local_x || new_y != local_y);
    local_x = new_x;
    local_y = new_y;
    local_dir = static_cast<protocol::Direction>(snapshot.get_direction());
    has_local_pos = true;
    if (snapshot.is_map_change()) {
        current_map_id = snapshot.get_mapa_id();
        others.clear();
        creatures.clear();
    }
}

void ClientGameState::apply_creature_position(const Snapshot& snapshot,
                                              const std::string& type) {
    const std::string& nick = snapshot.get_nick();
    CreatureView& cv = creatures[nick];
    const uint16_t new_x = snapshot.get_x();
    const uint16_t new_y = snapshot.get_y();
    cv.moved = (new_x != cv.x || new_y != cv.y);
    cv.key = nick;
    cv.type = type;
    cv.x = new_x;
    cv.y = new_y;
    cv.direction = static_cast<protocol::Direction>(snapshot.get_direction());
}

void ClientGameState::apply_other_player_position(const Snapshot& snapshot) {
    const std::string& nick = snapshot.get_nick();
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
    dead_entities.erase(nick);
    std::string type;
    if (classify_creature(nick, type)) {
        creatures.erase(nick);
        return;
    }
    others.erase(nick);
}

void ClientGameState::apply_player_stats(const Snapshot& snapshot) {
    const std::string& nick = snapshot.get_nick();

    if (nick == local_nick) {
        local_stats.raza = to_lower(snapshot.get_raza());
        local_stats.clase = to_lower(snapshot.get_clase());
        local_stats.nivel = snapshot.get_nivel();
        local_stats.vida = snapshot.get_vida();
        local_stats.vida_max = snapshot.get_vida_max();
        local_stats.mana = snapshot.get_mana();
        local_stats.mana_max = snapshot.get_mana_max();
        local_stats.experiencia = snapshot.get_experiencia();
        local_stats.oro = snapshot.get_oro();
        local_stats.constitucion = snapshot.get_constitucion();
        local_stats.inteligencia = snapshot.get_inteligencia();
        local_stats.fuerza = snapshot.get_fuerza();
        local_stats.agilidad = snapshot.get_agilidad();
        has_stats = true;
        has_error = false;
        return;
    }

    std::string type;
    if (classify_creature(nick, type)) {
        return;
    }

    const bool is_new = others.find(nick) == others.end();
    PlayerView& pv = others[nick];
    pv.nick = nick;
    pv.raza = to_lower(snapshot.get_raza());
    pv.clase = to_lower(snapshot.get_clase());
    if (is_new) {
        pv.x = snapshot.get_x();
        pv.y = snapshot.get_y();
        pv.direction =
            static_cast<protocol::Direction>(snapshot.get_direction());
    }
}

bool ClientGameState::resolve_entity_pos(const std::string& nick, uint16_t& x,
                                         uint16_t& y) const {
    if (nick == local_nick) {
        if (!has_local_pos) {
            return false;
        }
        x = local_x;
        y = local_y;
        return true;
    }
    auto ito = others.find(nick);
    if (ito != others.end()) {
        x = ito->second.x;
        y = ito->second.y;
        return true;
    }
    auto itc = creatures.find(nick);
    if (itc != creatures.end()) {
        x = itc->second.x;
        y = itc->second.y;
        return true;
    }
    return false;
}

void ClientGameState::apply_inventory_update(const Snapshot& snapshot) {
    if (snapshot.get_nick() != local_nick) return;

    const auto& items = snapshot.get_inventory_items();
    if (items.empty()) return;

    if (items.size() > 1) {
        inventory.resize(items.size());
        for (const auto& item : items) {
            if (item.slot_id < inventory.size()) {
                inventory[item.slot_id] =
                    {item.item, item.cantidad, item.equipado};
            }
        }
        return;
    }

    const auto& item = items[0];
    if (item.slot_id >= inventory.size()) {
        inventory.resize(item.slot_id + 1);
    }
    inventory[item.slot_id] = {item.item, item.cantidad, item.equipado};
}

bool ClientGameState::entity_at(uint16_t x, uint16_t y,
                                std::string& out_nick) const {
    for (const auto& [nick, cv] : creatures) {
        if (cv.x == x && cv.y == y) {
            out_nick = nick;
            return true;
        }
    }
    for (const auto& [nick, pv] : others) {
        if (pv.x == x && pv.y == y) {
            out_nick = nick;
            return true;
        }
    }
    return false;
}

void ClientGameState::apply_damage_event(const Snapshot& snapshot) {
    const std::string& attacker = snapshot.get_attacker();
    const std::string& target = snapshot.get_target();
    uint16_t x = 0;
    uint16_t y = 0;
    if (!resolve_entity_pos(target, x, y)) {
        return;
    }

    const bool target_es_jugador =
        (target == local_nick) || (others.count(target) > 0);

    FloatingKind kind;
    if (snapshot.is_critical()) {
        kind = FloatingKind::Crit;
    } else if (attacker == local_nick) {
        kind = FloatingKind::DamageDealt;
    } else if (target_es_jugador) {
        kind = FloatingKind::DamageReceived;
    } else {
        kind = FloatingKind::DamageDealt;
    }

    floating_events.push_back(
        {x, y, std::to_string(snapshot.get_damage()), kind});

    std::string creature_type;
    EffectKind atk_kind;

    if (snapshot.is_critical()) {
        atk_kind = EffectKind::AtaqueComunDorado;
    } else if (classify_creature(attacker, creature_type)) {
        atk_kind = EffectKind::AtaqueComunRojo;
    } else {
        atk_kind = EffectKind::AtaqueComunGris;
    }
    effect_spawns.push_back({x, y, atk_kind});

    const std::string dmg_str = std::to_string(snapshot.get_damage());

    if (attacker == local_nick) {
        push_chat("Combate", "Le hiciste " + dmg_str +
                  " de daño a " + format_chat_sender(target));
    } else if (target == local_nick) {
        push_chat("Combate", format_chat_sender(attacker) +
                  " te hizo " + dmg_str + " de daño");
    }
}

void ClientGameState::apply_dodge_event(const Snapshot& snapshot) {
    const std::string& target = snapshot.get_target();
    uint16_t x = 0;
    uint16_t y = 0;
    if (!resolve_entity_pos(target, x, y)) {
        return;
    }
    floating_events.push_back({x, y, "Esquivó", FloatingKind::Dodge});

    if (target == local_nick) {
        push_chat("Combate", "Esquivaste el ataque");
    } else {
        push_chat("Combate", format_chat_sender(target) + " esquivó tu ataque");
    }
}

void ClientGameState::apply_death_event(const Snapshot& snapshot) {
    const std::string& target = snapshot.get_target();
    uint16_t x = 0;
    uint16_t y = 0;
    if (!resolve_entity_pos(target, x, y)) {
        return;
    }
    floating_events.push_back({x, y, "¡Murió!", FloatingKind::Death});
    effect_spawns.push_back({x, y, EffectKind::EfectoMorir});
    dead_entities.insert(target);
}

void ClientGameState::apply_meditation_status(const Snapshot& snapshot) {
    const std::string& nick = snapshot.get_nick();
    if (snapshot.is_meditating()) {
        meditating_entities.insert(nick);
    } else {
        meditating_entities.erase(nick);
    }
}

bool ClientGameState::is_meditating(const std::string& nick) const {
    return meditating_entities.count(nick) > 0;
}

void ClientGameState::push_chat(const std::string& from,
                                 const std::string& text) {
    chat_messages.push_back({from, text});
    if (chat_messages.size() > MAX_CHAT_MESSAGES) {
        chat_messages.erase(chat_messages.begin());
    }
}

std::string ClientGameState::format_chat_sender(
        const std::string& nick) const {
    static const std::unordered_set<std::string_view> CREATURE_TYPES = {
        keys::GOBLIN, keys::ESQUELETO, keys::ZOMBIE, keys::ARANA,
        keys::ORCO, keys::GOLEM, keys::BANQUERO, keys::COMERCIANTE,
        keys::SACERDOTE};

    const auto sep = nick.rfind('_');
    if (sep == std::string::npos || sep == 0) {
        return nick;
    }
    for (size_t i = sep + 1; i < nick.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(nick[i]))) {
            return nick;
        }
    }
    const std::string prefix = to_lower(nick.substr(0, sep));
    if (CREATURE_TYPES.find(prefix) == CREATURE_TYPES.end()) {
        return nick;
    }
    return prefix;
}

void ClientGameState::apply_chat_message(const Snapshot& snapshot) {
    push_chat(snapshot.get_nick(), snapshot.get_text());
}

const std::vector<ChatMessage>& ClientGameState::get_chat_messages() const {
    return chat_messages;
}

void ClientGameState::apply_item_event(const Snapshot& snapshot) {
    const uint16_t x = snapshot.get_x();
    const uint16_t y = snapshot.get_y();
    const uint32_t key = (static_cast<uint32_t>(x) << 16) | y;
    if (snapshot.get_item_action() ==
        static_cast<uint8_t>(protocol::ItemEventAction::DROP)) {
        floor_items[key] =
            FloorItem{x, y, snapshot.get_item_name(), snapshot.get_amount()};
    } else {
        floor_items.erase(key);
    }
}

const std::unordered_map<uint32_t, FloorItem>&
ClientGameState::get_floor_items() const {
    return floor_items;
}

void ClientGameState::apply_error_message(const Snapshot& snapshot) {
    const std::string& nick = snapshot.get_nick();
    if (!nick.empty() && nick != local_nick) {
        return;
    }
    last_error = snapshot.get_text();
    has_error = true;
    ++error_seq;
}

bool ClientGameState::has_local_position() const { return has_local_pos; }

uint16_t ClientGameState::get_local_x() const { return local_x; }

uint16_t ClientGameState::get_local_y() const { return local_y; }

protocol::Direction ClientGameState::get_local_dir() const { return local_dir; }

bool ClientGameState::get_local_moved() const { return local_moved; }

const std::string& ClientGameState::get_local_nick() const {
    return local_nick;
}

bool ClientGameState::has_local_stats() const { return has_stats; }

const PlayerStats& ClientGameState::get_local_stats() const {
    return local_stats;
}
uint16_t ClientGameState::get_current_map_id() const { return current_map_id; }

bool ClientGameState::has_pending_error() const { return has_error; }

const std::string& ClientGameState::get_last_error() const {
    return last_error;
}
uint32_t ClientGameState::get_error_seq() const { return error_seq; }

const std::vector<InventorySlotView>& ClientGameState::get_inventory() const {
    return inventory;
}

const std::unordered_map<std::string, PlayerView>&
                    ClientGameState::get_others() const {
                        return others;
                    }

const std::vector<FloatingEvent>&
                    ClientGameState::get_floating_events() const {
                        return floating_events;
                    }

const std::vector<EffectSpawn>& ClientGameState::get_effect_spawns() const {
    return effect_spawns;
}

const std::unordered_map<std::string, CreatureView>&
                    ClientGameState::get_creatures() const {
                        return creatures;
                    }

bool ClientGameState::is_dead(const std::string& nick) const {
    return dead_entities.count(nick) > 0;
}

int ClientGameState::get_map_width() const { return map_width; }

int ClientGameState::get_map_height() const { return map_height; }
