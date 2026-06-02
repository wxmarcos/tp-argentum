#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <cstdint>
#include <string>

#include "common/protocol_defs.h"

class Socket;

class Snapshot {
private:
    protocol::ServerOpcode opcode;

    std::string nick;

    std::string attacker;
    std::string target;

    uint16_t x = 0;
    uint16_t y = 0;

    uint8_t direction = 0;

    uint16_t damage = 0;
    bool critical = false;

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
};

#endif