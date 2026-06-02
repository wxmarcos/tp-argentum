#pragma once

#include <cstdint>
#include <string>

#include "common/protocol_defs.h"

class Socket;

class Command {
private:
    uint16_t player_id;
    protocol::ClientOpcode type;

    uint8_t direction = 0;
    uint16_t item_id = 0;
    uint16_t slot = 0;
    uint32_t amount = 0;

    std::string nick;
    std::string text;
    std::string clan_name;
    std::string raza;
    std::string clase;

public:
    explicit Command(uint16_t player_id, protocol::ClientOpcode type);

    static Command recv(Socket& socket, uint16_t player_id);
    void send(Socket& socket) const;

    static Command login(const std::string& nick);
    static Command create_character(
        const std::string& nick,
        const std::string& raza,
        const std::string& clase);

    static Command move(uint8_t direction);
    static Command attack(const std::string& nick);
    static Command disconnect();

    bool is_disconnect() const;

    protocol::ClientOpcode get_type() const;
    uint16_t get_player_id() const;

    uint8_t get_direction() const;
    uint16_t get_item_id() const;
    uint16_t get_slot() const;
    uint32_t get_amount() const;

    const std::string& get_nick() const;
    const std::string& get_text() const;
    const std::string& get_clan_name() const;
    const std::string& get_raza() const;
    const std::string& get_clase() const;
};