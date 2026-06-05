#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <cstdint>
#include <string>
#include <vector>
#include "common/protocol_defs.h"

class Socket;
struct InventorySnapshotItem{
    uint16_t slot_id;
    std::string item;
    uint16_t cantidad;  
    bool equipado;
};
class Snapshot {
private:
    protocol::ServerOpcode opcode;

    std::string nick;
    std::string text;
    std::string attacker;
    std::string target;
    std::string raza;
    std::string clase;

    uint16_t x = 0;
    uint16_t y = 0;

    uint8_t direction = 0;
    uint16_t mapa_id = 0;

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
    uint16_t damage = 0;
    bool critical = false;
    bool meditating = false;
    std::vector<InventorySnapshotItem> inventory_items;
public:
    Snapshot(
        protocol::ServerOpcode opcode,
        const std::string& nick,
        uint16_t x = 0,
        uint16_t y = 0,
        uint8_t direction = 0);

    static Snapshot entity_created(
        const std::string& nick,
        uint16_t x,
        uint16_t y,
        uint8_t direction);
        
    static Snapshot entity_login(
        const std::string& nick,
        uint16_t x,
        uint16_t y,
        uint8_t direction);

    static Snapshot entity_move(
        const std::string& nick,
        uint16_t x,
        uint16_t y,
        uint8_t direction);

    static Snapshot entity_remove(
        const std::string& nick);

    static Snapshot damage_event(
        const std::string& attacker,
        const std::string& target,
        uint16_t damage,
        bool critical);

    static Snapshot dodge_event(
        const std::string& attacker,
        const std::string& target);

    static Snapshot death_event(
        const std::string& target);

    void send(Socket& socket) const;

    static Snapshot recv(Socket& socket);

    protocol::ServerOpcode get_opcode() const;

    static Snapshot error_message(
        const std::string& nick,
        const std::string& text
    );

    bool is_error_message() const;

    static Snapshot chat_message(
        const std::string& from,
        const std::string& to,
        const std::string& text
    );
    static Snapshot meditation_status(
        const std::string& nick,
        bool started
    );
    static Snapshot player_stats(
        const std::string& nick,
        const std::string& raza,
        const std::string& clase,
        uint16_t mapa_id,
        uint16_t x,
        uint16_t y,
        uint8_t direction,
        uint16_t nivel,
        uint16_t vida,
        uint16_t vida_max,
        uint16_t mana,
        uint16_t mana_max,
        uint32_t experiencia,
        uint32_t oro,
        uint16_t constitucion,
        uint16_t inteligencia,
        uint16_t fuerza,
        uint16_t agilidad
    );
    static Snapshot inventory_update(
        const std::string& nick,
        const std::vector<InventorySnapshotItem>& items
    );

    bool is_inventory_update() const;

    const std::vector<InventorySnapshotItem>& get_inventory_items() const;
    const std::string& get_raza() const;
    const std::string& get_clase() const;
    static Snapshot map_change(
    const std::string& nick,
        uint16_t mapa_id,
        uint16_t x,
        uint16_t y,
        uint8_t direction
    );

    bool is_map_change() const;
    uint16_t get_mapa_id() const;
    uint16_t get_nivel() const;

    uint16_t get_vida() const;
    uint16_t get_vida_max() const;

    uint16_t get_mana() const;
    uint16_t get_mana_max() const;

    uint32_t get_experiencia() const;
    uint32_t get_oro() const;

    uint16_t get_constitucion() const;
    uint16_t get_inteligencia() const;
    uint16_t get_fuerza() const;
    uint16_t get_agilidad() const;
    bool is_player_stats() const;
    bool is_chat_message() const;
    const std::string& get_text() const;

    bool is_entity_created() const;
    bool is_entity_login() const;
    bool is_entity_move() const;
    bool is_entity_remove() const;

    bool is_damage_event() const;
    bool is_dodge_event() const;
    bool is_death_event() const;

    const std::string& get_nick() const;

    const std::string& get_attacker() const;
    const std::string& get_target() const;

    uint16_t get_x() const;
    uint16_t get_y() const;
    uint8_t get_direction() const;

    uint16_t get_damage() const;
    bool is_critical() const;
    bool is_meditating() const;
    bool is_meditation_status() const;
};

#endif