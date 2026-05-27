#include "command.h"
#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <stdexcept>
#include <vector>

#include "network/socket.h"


static uint8_t read_u8(const std::vector<uint8_t>& data, size_t& offset) {

    if (offset + 1 > data.size()) {
        throw std::runtime_error("payload incompleto leyendo uint8");
    }

    return data[offset++];
}

static uint16_t read_u16(const std::vector<uint8_t>& data, size_t& offset) {

    if (offset + 2 > data.size()) {
        throw std::runtime_error("payload incompleto leyendo uint16");
    }

    uint16_t net_value;
    std::memcpy(&net_value, data.data() + offset, sizeof(net_value));
    offset += 2;

    return ntohs(net_value);
}

static uint32_t read_u32(const std::vector<uint8_t>& data, size_t& offset) {

    if (offset + 4 > data.size()) {
        throw std::runtime_error("payload incompleto leyendo uint32");
    }

    uint32_t net_value;
    std::memcpy(&net_value, data.data() + offset, sizeof(net_value));
    offset += 4;

    return ntohl(net_value);
}


static std::string read_string(
    const std::vector<uint8_t>& data,
    size_t& offset) {

    uint16_t len = read_u16(data, offset);

    if (offset + len > data.size()) {
        throw std::runtime_error("payload incompleto leyendo string");
    }

    std::string value(
        reinterpret_cast<const char*>(data.data() + offset),
        len);

    offset += len;

    return value;
}

Command::Command(uint16_t player_id, CommandType type)
    : player_id(player_id),
      type(type) {}

Command Command::recv(Socket& socket, uint16_t player_id) {

    uint8_t opcode_raw;
    uint16_t net_payload_size;

    int received = socket.recvall(&opcode_raw, sizeof(opcode_raw));

    if (received == 0) {
        return Command(player_id, CommandType::Disconnect);
    }

    received = socket.recvall(
        &net_payload_size,
        sizeof(net_payload_size));

    if (received == 0) {
        return Command(player_id, CommandType::Disconnect);
    }

    uint16_t payload_size = ntohs(net_payload_size);
    // DEBUG
    std::cout
    << "[Protocol] opcode="
    << static_cast<int>(opcode_raw)
    << " payload_size="
    << payload_size
    << "\n";

    std::vector<uint8_t> payload(payload_size);

    if (payload_size > 0) {
        received = socket.recvall(payload.data(), payload_size);

        if (received == 0) {
            return Command(player_id, CommandType::Disconnect);
        }
    }

    auto type = static_cast<CommandType>(opcode_raw);

    Command cmd(player_id, type);

    size_t offset = 0;

    switch (type) {

        case CommandType::Move:
            cmd.direction = read_u8(payload, offset);
            break;

        case CommandType::Attack:
            cmd.target_id = read_u32(payload, offset);
            break;

        case CommandType::Meditate:
        case CommandType::Resurrect:
        case CommandType::Heal:
        case CommandType::PickItem:
        case CommandType::ClanReview:
        case CommandType::ClanLeave:
        case CommandType::Disconnect:
            break;

        case CommandType::DropItem:
        case CommandType::EquipItem:
        case CommandType::BuyItem:
        case CommandType::SellItem:
            cmd.item_id = read_u16(payload, offset);
            break;

        case CommandType::DepositItem:
        case CommandType::WithdrawItem:
            cmd.item_id = read_u16(payload, offset);
            cmd.amount = read_u32(payload, offset);
            break;

        case CommandType::PrivateMessage:
            cmd.nick = read_string(payload, offset);
            cmd.text = read_string(payload, offset);
            break;

        case CommandType::ClanCreate:
        case CommandType::ClanJoin:
            cmd.clan_name = read_string(payload, offset);
            break;

        case CommandType::ClanAccept:
        case CommandType::ClanReject:
        case CommandType::ClanBan:
        case CommandType::ClanKick:
            cmd.nick = read_string(payload, offset);
            break;

        default:
            throw std::runtime_error("opcode desconocido");
    }

    if (offset != payload.size()) {
        throw std::runtime_error("payload con bytes extra");
    }

    return cmd;
}

bool Command::is_disconnect() const {
    return type == CommandType::Disconnect;
}

CommandType Command::get_type() const {
    return type;
}

uint16_t Command::get_player_id() const {
    return player_id;
}

uint8_t Command::get_direction() const {
    return direction;
}

uint32_t Command::get_target() const {
    return target_id;
}

uint16_t Command::get_item_id() const {
    return item_id;
}

uint16_t Command::get_slot() const {
    return slot;
}

uint32_t Command::get_amount() const {
    return amount;
}

const std::string& Command::get_nick() const {
    return nick;
}

const std::string& Command::get_text() const {
    return text;
}

const std::string& Command::get_clan_name() const {
    return clan_name;
}