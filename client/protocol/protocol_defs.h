#ifndef CLIENT_PROTOCOL_DEFS_H
#define CLIENT_PROTOCOL_DEFS_H

#include <cstdint>
#include <string>

namespace protocol {

enum class Direction : uint8_t {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3,
};

enum class CommandType : uint8_t {
    MOVE = 0,
    UNKNOWN = 255,
};

namespace wire {
inline constexpr const char* MOVE = "MOVE";

inline constexpr const char* DIR_NORTH = "N";
inline constexpr const char* DIR_EAST = "E";
inline constexpr const char* DIR_SOUTH = "S";
inline constexpr const char* DIR_WEST = "W";

inline constexpr char FIELD_SEP = ' ';
inline constexpr char MSG_END = '\n';
}

}

#endif
