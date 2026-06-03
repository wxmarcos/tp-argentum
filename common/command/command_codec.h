#ifndef COMMON_COMMAND_CODEC_H
#define COMMON_COMMAND_CODEC_H

#include <vector>
#include "command.h"
#include "common/protocol_defs.h"

Command parse_command_payload(
    const std::vector<uint8_t>& payload,
    protocol::ClientOpcode type,
    uint16_t player_id);

std::vector<uint8_t> build_command_payload(const Command& command);

#endif // COMMON_COMMAND_CODEC_H
