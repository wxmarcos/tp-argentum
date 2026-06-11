#pragma once

#include <cstdint>

namespace protocol {

inline constexpr uint8_t OPCODE_SIZE = 1;
inline constexpr uint8_t PAYLOAD_SIZE_SIZE = 2;
inline constexpr uint16_t MAX_PAYLOAD_SIZE = 4096;

enum class Direction : uint8_t { NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3 };
enum class ItemEventAction : uint8_t { DROP = 0, PICK = 1 };

enum class ClientOpcode : uint8_t {
    LOGIN = 0,
    CREATE_CHARACTER = 1,

    MOVE = 2,
    ATTACK = 3,

    MEDITATE = 4,
    RESURRECT = 5,
    HEAL = 6,

    PICK_ITEM = 10,
    DROP_ITEM = 11,
    EQUIP_ITEM = 12,
    BUY_ITEM = 13,
    SELL_ITEM = 14,
    DEPOSIT_ITEM = 15,
    WITHDRAW_ITEM = 16,
    DEPOSIT_GOLD = 17,
    WITHDRAW_GOLD = 18,

    PRIVATE_MESSAGE = 30,

    CLAN_CREATE = 40,
    CLAN_JOIN = 41,
    CLAN_REVIEW = 42,
    CLAN_ACCEPT = 43,
    CLAN_REJECT = 44,
    CLAN_BAN = 45,
    CLAN_KICK = 46,
    CLAN_LEAVE = 47,

    CHEAT_GOD = 100,
    CHEAT_MANA = 101,
    CHEAT_DIE = 102,
    CHEAT_RESURRECT = 103,

    DISCONNECT = 255
};

enum class ServerOpcode : uint8_t {
    SNAPSHOT = 1,
    ENTITY_SPAWN = 2,
    ENTITY_MOVE = 3,
    ENTITY_REMOVE = 4,
    ENTITY_CREATED = 5,
    ENTITY_LOGIN = 6,

    PLAYER_STATS = 10,
    INVENTORY_UPDATE = 11,
    MEDITATION_STATUS = 12,
    MAP_CHANGE = 13,
    CHEAT_STATUS = 14,
    CHAT_MESSAGE = 30,
    DAMAGE_EVENT = 40,
    DODGE_EVENT = 41,
    DEATH_EVENT = 42,
    ITEM_EVENT = 43,

    ERROR_MESSAGE = 250
};

}  // namespace protocol
