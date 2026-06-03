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
    << "[Command] opcode="
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
    std::cout << "[Command] payload hex: ";

    for (uint8_t b : payload) {
        printf("%02X ", b);
    }

    std::cout << "\n";
    auto type = static_cast<CommandType>(opcode_raw);

    Command cmd(player_id, type);

    size_t offset = 0;

    switch (type) {
        case CommandType::CreateCharacter:

            cmd.nick =
                read_string(payload, offset);

            cmd.raza =
                read_string(payload, offset);

            cmd.clase =
                read_string(payload, offset);

            break;
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
            //DEBUG
            std::cout << "[Command] OPCODE DESCONOCIDO: "
                 << static_cast<int>(type)
                 << "\n";
            throw std::runtime_error("opcode desconocido");
    }

    if (offset != payload.size()) {
        throw std::runtime_error("payload con bytes extra");
    }

    return cmd;
}
void Command::send(Socket& socket) const {

    uint8_t opcode = static_cast<uint8_t>(type);

    std::vector<uint8_t> payload;

    auto push_u8 = [&](uint8_t value) {
        payload.push_back(value);
    };

    auto push_u16 = [&](uint16_t value) {
        uint16_t net_value = htons(value);
        auto* bytes = reinterpret_cast<uint8_t*>(&net_value);

        payload.push_back(bytes[0]);
        payload.push_back(bytes[1]);
    };

    auto push_u32 = [&](uint32_t value) {
        uint32_t net_value = htonl(value);
        auto* bytes = reinterpret_cast<uint8_t*>(&net_value);

        payload.push_back(bytes[0]);
        payload.push_back(bytes[1]);
        payload.push_back(bytes[2]);
        payload.push_back(bytes[3]);
    };

    auto push_string = [&](const std::string& value) {
        push_u16(static_cast<uint16_t>(value.size()));
        payload.insert(
            payload.end(),
            value.begin(),
            value.end());
    };

    switch (type) {
        case CommandType::CreateCharacter:
            push_string(nick);
            push_string(raza);
            push_string(clase);
            break;
        case CommandType::Move:
            push_u8(direction);
            break;

        case CommandType::Attack:
            push_u32(target_id);
            break;

        case CommandType::DropItem:
        case CommandType::EquipItem:
        case CommandType::BuyItem:
        case CommandType::SellItem:
            push_u16(item_id);
            break;

        case CommandType::DepositItem:
        case CommandType::WithdrawItem:
            push_u16(item_id);
            push_u32(amount);
            break;

        case CommandType::PrivateMessage:
            push_string(nick);
            push_string(text);
            break;

        case CommandType::ClanCreate:
        case CommandType::ClanJoin:
            push_string(clan_name);
            break;

        case CommandType::ClanAccept:
        case CommandType::ClanReject:
        case CommandType::ClanBan:
        case CommandType::ClanKick:
            push_string(nick);
            break;

        case CommandType::Meditate:
        case CommandType::Resurrect:
        case CommandType::Heal:
        case CommandType::PickItem:
        case CommandType::ClanReview:
        case CommandType::ClanLeave:
        case CommandType::Disconnect:
            break;
    }

    uint16_t net_payload_size =
        htons(static_cast<uint16_t>(payload.size()));

    socket.sendall(&opcode, sizeof(opcode));

    socket.sendall(
        &net_payload_size,
        sizeof(net_payload_size));

    if (!payload.empty()) {
        socket.sendall(
            payload.data(),
            payload.size());
    }
}

Command Command::move(uint8_t direction) {
    Command cmd(0, CommandType::Move);
    cmd.direction = direction;
    return cmd;
}

Command Command::attack(uint32_t target_id) {
    Command cmd(0, CommandType::Attack);
    cmd.target_id = target_id;
    return cmd;
}

Command Command::disconnect() {
    return Command(0, CommandType::Disconnect);
}

Command Command::create_character(const std::string& nick, const std::string& raza, const std::string& clase) {
    Command cmd(0, CommandType::CreateCharacter);
    cmd.nick = nick;
    cmd.raza = raza;
    cmd.clase = clase;
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

const std::string& Command::get_raza() const {
    return raza;
}

const std::string& Command::get_clase() const {
    return clase;
}