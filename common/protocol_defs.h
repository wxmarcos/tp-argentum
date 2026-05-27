#pragma once

#include <cstdint>
inline constexpr uint8_t OPCODE_SIZE = 1;
inline constexpr uint8_t PAYLOAD_SIZE_SIZE = 2;
inline constexpr uint16_t MAX_PAYLOAD_SIZE = 4096;

namespace protocol {

enum class Direction : uint8_t {
    NORTH = 0,
    EAST  = 1,
    SOUTH = 2,
    WEST  = 3
};

enum class ClientOpcode : uint8_t {
    MOVE = 1,
    ATTACK = 2,

    MEDITATE = 3,
    RESURRECT = 4,
    HEAL = 5,

    PICK_ITEM = 10,
    DROP_ITEM = 11,
    EQUIP_ITEM = 12,
    BUY_ITEM = 13,
    SELL_ITEM = 14,
    DEPOSIT_ITEM = 15,
    WITHDRAW_ITEM = 16,

    PRIVATE_MESSAGE = 30,

    CLAN_CREATE = 40,
    CLAN_JOIN = 41,
    CLAN_REVIEW = 42,
    CLAN_ACCEPT = 43,
    CLAN_REJECT = 44,
    CLAN_BAN = 45,
    CLAN_KICK = 46,
    CLAN_LEAVE = 47,

    DISCONNECT = 255
};

enum class ServerOpcode : uint8_t {
    SNAPSHOT = 1,
    ENTITY_SPAWN = 2,
    ENTITY_MOVE = 3,
    ENTITY_REMOVE = 4,

    PLAYER_STATS = 10,
    INVENTORY_UPDATE = 11,

    CHAT_MESSAGE = 30,
    DAMAGE_EVENT = 40,
    DODGE_EVENT = 41,
    DEATH_EVENT = 42,

    ERROR_MESSAGE = 250
};

}