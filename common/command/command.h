#pragma once

#include <cstdint>
#include <string>
#include <vector>

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

    friend Command parse_command_payload(const std::vector<uint8_t>& payload,
                                         protocol::ClientOpcode type,
                                         uint16_t player_id);

    friend Command parse_login(const std::vector<uint8_t>& payload,
                               size_t& offset, uint16_t player_id);

    friend Command parse_create_character(const std::vector<uint8_t>& payload,
                                          size_t& offset, uint16_t player_id);

    friend Command parse_move(const std::vector<uint8_t>& payload,
                              size_t& offset, uint16_t player_id);

    friend Command parse_attack(const std::vector<uint8_t>& payload,
                                size_t& offset, uint16_t player_id);

    friend Command parse_item_id(const std::vector<uint8_t>& payload,
                                 size_t& offset, protocol::ClientOpcode opcode,
                                 uint16_t player_id);

    friend Command parse_item_and_amount(const std::vector<uint8_t>& payload,
                                         size_t& offset,
                                         protocol::ClientOpcode opcode,
                                         uint16_t player_id);

    friend Command parse_slot(const std::vector<uint8_t>& payload,
                              size_t& offset, protocol::ClientOpcode opcode,
                              uint16_t player_id);
    friend Command parse_slot_and_amount(const std::vector<uint8_t>& payload,
                                         size_t& offset,
                                         protocol::ClientOpcode opcode,
                                         uint16_t player_id);
    friend Command parse_private_message(const std::vector<uint8_t>& payload,
                                         size_t& offset, uint16_t player_id);

    friend Command parse_clan_name(const std::vector<uint8_t>& payload,
                                   size_t& offset,
                                   protocol::ClientOpcode opcode,
                                   uint16_t player_id);

    friend Command parse_nick_only(const std::vector<uint8_t>& payload,
                                   size_t& offset,
                                   protocol::ClientOpcode opcode,
                                   uint16_t player_id);

    friend Command parse_text(const std::vector<uint8_t>& payload,
                              size_t& offset,
                              protocol::ClientOpcode opcode,
                              uint16_t player_id);

    friend Command parse_amount(const std::vector<uint8_t>& payload,
                                size_t& offset,
                                protocol::ClientOpcode opcode,
                                uint16_t player_id);

    friend std::vector<uint8_t> build_command_payload(const Command& command);

    static Command recv(Socket& socket, uint16_t player_id);
    void send(Socket& socket) const;

    static Command login(const std::string& nick);
    static Command create_character(const std::string& nick,
                                    const std::string& raza,
                                    const std::string& clase);

    static Command move(uint8_t direction);
    static Command attack(const std::string& nick);
    static Command buy_item(const std::string& itemName);
    static Command deposit_gold(uint32_t amount);
    static Command withdraw_gold(uint32_t amount);
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
