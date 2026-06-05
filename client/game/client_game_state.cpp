#include "game/client_game_state.h"

#include <cctype>
#include <unordered_set>

#include "common/snapshot/snapshot.h"

namespace {
bool classify_creature(const std::string& nick, std::string& type) {
    static const std::unordered_set<std::string> KNOWN_TYPES = {
        "goblin", "esqueleto", "zombie", "arana", "orco", "golem",
        "banquero", "comerciante", "sacerdote"};

    const auto sep = nick.rfind('_');
    if (sep == std::string::npos || sep == 0 || sep + 1 >= nick.size()) {
        return false;
    }
    for (size_t i = sep + 1; i < nick.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(nick[i]))) {
            return false;
        }
    }
    std::string prefix = nick.substr(0, sep);
    for (char& c : prefix) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    if (KNOWN_TYPES.find(prefix) == KNOWN_TYPES.end()) {
        return false;
    }
    type = prefix;
    return true;
}

std::string to_lower(std::string s) {
    for (char& c : s) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return s;
}
}

ClientGameState::ClientGameState(const std::string& local_nick, int map_width,
                                 int map_height):
    local_nick(local_nick),
    has_local_pos(false), local_x(0), local_y(0),
    local_dir(protocol::Direction::SOUTH), local_moved(false),
    current_map_id(0), has_stats(false), has_error(false),
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
    if (snapshot.is_entity_created() || snapshot.is_entity_login() ||
        snapshot.is_entity_move() || snapshot.is_map_change()) {
        apply_entity_position(snapshot);
        return;
    }

    if (snapshot.is_entity_remove()) {
        apply_entity_remove(snapshot);
        return;
    }

    if (snapshot.is_player_stats()) {
        apply_player_stats(snapshot);
        return;
    }

    if (snapshot.is_inventory_update()) {
        apply_inventory_update(snapshot);
        return;
    }

    if (snapshot.is_damage_event()) {
        apply_damage_event(snapshot);
        return;
    }

    if (snapshot.is_dodge_event()) {
        apply_dodge_event(snapshot);
        return;
    }

    if (snapshot.is_death_event()) {
        apply_death_event(snapshot);
        return;
    }

    if (snapshot.is_meditation_status()) {
        apply_meditation_status(snapshot);
        return;
    }

    if (snapshot.is_chat_message()) {
        apply_chat_message(snapshot);
        return;
    }

    if (snapshot.is_error_message()) {
        apply_error_message(snapshot);
        return;
    }
}

void ClientGameState::apply_entity_position(const Snapshot& snapshot) {
    const std::string& nick = snapshot.get_nick();

    if (nick == local_nick) {
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

void ClientGameState::apply_inventory_update(const Snapshot&) {
    // TODO
}

void ClientGameState::apply_damage_event(const Snapshot&) {
    // TODO
}

void ClientGameState::apply_dodge_event(const Snapshot&) {
    // TODO
}

void ClientGameState::apply_death_event(const Snapshot&) {
    // TODO
}

void ClientGameState::apply_meditation_status(const Snapshot&) {
    // TODO
}

void ClientGameState::apply_chat_message(const Snapshot&) {
    // TODO
}

void ClientGameState::apply_error_message(const Snapshot& snapshot) {
    last_error = snapshot.get_text();
    has_error = true;
}
