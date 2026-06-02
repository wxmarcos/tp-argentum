#pragma once

#include <cstdint>
#include <string>

#include "common/protocol_defs.h"

class Socket;

enum class CommandType : uint8_t {
    CreateCharacter = 0,
    Move = 1,
    Attack = 2,
    Meditate = 3,
    Resurrect = 4,
    Heal = 5,

    PickItem = 10,
    DropItem = 11,
    EquipItem = 12,
    BuyItem = 13,
    SellItem = 14,
    DepositItem = 15,
    WithdrawItem = 16,

    PrivateMessage = 30,

    ClanCreate = 40,
    ClanJoin = 41,
    ClanReview = 42,
    ClanAccept = 43,
    ClanReject = 44,
    ClanBan = 45,
    ClanKick = 46,
    ClanLeave = 47,

    Disconnect = 255
};

class Command {
private:
    uint16_t player_id;
    CommandType type;

    uint8_t direction = 0;
    uint32_t target_id = 0;
    uint16_t item_id = 0;
    uint16_t slot = 0;
    uint32_t amount = 0;

    std::string nick;
    std::string text;
    std::string clan_name;
    std::string raza;
    std::string clase;

public:
    explicit Command(uint16_t player_id, CommandType type);

    static Command recv(Socket& socket, uint16_t player_id);
    void send(Socket& socket) const;

    static Command create_character(const std::string& nick, const std::string& raza, const std::string& clase);
    static Command move(uint8_t direction);
    static Command attack(const std::string& nick);
    static Command disconnect();
    
    bool is_disconnect() const;

    CommandType get_type() const;
    uint16_t get_player_id() const;

    uint8_t get_direction() const;
    uint32_t get_target() const;
    uint16_t get_item_id() const;
    uint16_t get_slot() const;
    uint32_t get_amount() const;

    const std::string& get_nick() const;
    const std::string& get_text() const;
    const std::string& get_clan_name() const;
    const std::string& get_raza() const;
    const std::string& get_clase() const;
};