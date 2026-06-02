#include "command.h"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
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

static std::string read_string(const std::vector<uint8_t>& data, size_t& offset) {
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

static void validate_no_extra_bytes(const std::vector<uint8_t>& payload, size_t offset) {
    if (offset != payload.size()) {
        throw std::runtime_error("payload con bytes extra");
    }
}

Command::Command(uint16_t player_id, protocol::ClientOpcode type)
    : player_id(player_id),
      type(type) {}

Command Command::recv(Socket& socket, uint16_t player_id) {
    uint8_t opcode_raw;
    uint16_t net_payload_size;

    int received = socket.recvall(&opcode_raw, sizeof(opcode_raw));

    if (received == 0) {
        return Command(player_id, protocol::ClientOpcode::DISCONNECT);
    }

    received = socket.recvall(&net_payload_size, sizeof(net_payload_size));

    if (received == 0) {
        return Command(player_id, protocol::ClientOpcode::DISCONNECT);
    }

    uint16_t payload_size = ntohs(net_payload_size);

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
            return Command(player_id, protocol::ClientOpcode::DISCONNECT);
        }
    }

    std::cout << "[Command] payload hex: ";

    for (uint8_t b : payload) {
        printf("%02X ", b);
    }

    std::cout << "\n";

    auto type = static_cast<protocol::ClientOpcode>(opcode_raw);

    Command cmd(player_id, type);

    size_t offset = 0;

    switch (type) {
        case protocol::ClientOpcode::LOGIN: {
            cmd.nick = read_string(payload, offset);
            break;
        }

        case protocol::ClientOpcode::CREATE_CHARACTER: {
            cmd.nick = read_string(payload, offset);
            cmd.raza = read_string(payload, offset);
            cmd.clase = read_string(payload, offset);
            break;
        }

        case protocol::ClientOpcode::MOVE: {
            cmd.direction = read_u8(payload, offset);
            break;
        }

        case protocol::ClientOpcode::ATTACK: {
            cmd.nick = read_string(payload, offset);
            break;
        }

        case protocol::ClientOpcode::MEDITATE:
        case protocol::ClientOpcode::RESURRECT:
        case protocol::ClientOpcode::HEAL:
        case protocol::ClientOpcode::PICK_ITEM:
        case protocol::ClientOpcode::CLAN_REVIEW:
        case protocol::ClientOpcode::CLAN_LEAVE:
        case protocol::ClientOpcode::DISCONNECT: {
            break;
        }

        case protocol::ClientOpcode::DROP_ITEM:
        case protocol::ClientOpcode::EQUIP_ITEM: {
            cmd.item_id = read_u16(payload, offset);
            break;
        }

        case protocol::ClientOpcode::BUY_ITEM:
        case protocol::ClientOpcode::SELL_ITEM:
        case protocol::ClientOpcode::DEPOSIT_ITEM:
        case protocol::ClientOpcode::WITHDRAW_ITEM: {
            cmd.item_id = read_u16(payload, offset);
            cmd.amount = read_u32(payload, offset);
            break;
        }

        case protocol::ClientOpcode::PRIVATE_MESSAGE: {
            cmd.nick = read_string(payload, offset);
            cmd.text = read_string(payload, offset);
            break;
        }

        case protocol::ClientOpcode::CLAN_CREATE:
        case protocol::ClientOpcode::CLAN_JOIN: {
            cmd.clan_name = read_string(payload, offset);
            break;
        }

        case protocol::ClientOpcode::CLAN_ACCEPT:
        case protocol::ClientOpcode::CLAN_REJECT:
        case protocol::ClientOpcode::CLAN_BAN:
        case protocol::ClientOpcode::CLAN_KICK: {
            cmd.nick = read_string(payload, offset);
            break;
        }

        default: {
            std::cout
                << "[Command] OPCODE DESCONOCIDO: "
                << static_cast<int>(opcode_raw)
                << "\n";

            throw std::runtime_error("opcode desconocido");
        }
    }

    validate_no_extra_bytes(payload, offset);

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
        payload.insert(payload.end(), value.begin(), value.end());
    };

    switch (type) {
        case protocol::ClientOpcode::LOGIN: {
            push_string(nick);
            break;
        }

        case protocol::ClientOpcode::CREATE_CHARACTER: {
            push_string(nick);
            push_string(raza);
            push_string(clase);
            break;
        }

        case protocol::ClientOpcode::MOVE: {
            push_u8(direction);
            break;
        }

        case protocol::ClientOpcode::ATTACK: {
            push_string(nick);
            break;
        }

        case protocol::ClientOpcode::DROP_ITEM:
        case protocol::ClientOpcode::EQUIP_ITEM: {
            push_u16(item_id);
            break;
        }

        case protocol::ClientOpcode::BUY_ITEM:
        case protocol::ClientOpcode::SELL_ITEM:
        case protocol::ClientOpcode::DEPOSIT_ITEM:
        case protocol::ClientOpcode::WITHDRAW_ITEM: {
            push_u16(item_id);
            push_u32(amount);
            break;
        }

        case protocol::ClientOpcode::PRIVATE_MESSAGE: {
            push_string(nick);
            push_string(text);
            break;
        }

        case protocol::ClientOpcode::CLAN_CREATE:
        case protocol::ClientOpcode::CLAN_JOIN: {
            push_string(clan_name);
            break;
        }

        case protocol::ClientOpcode::CLAN_ACCEPT:
        case protocol::ClientOpcode::CLAN_REJECT:
        case protocol::ClientOpcode::CLAN_BAN:
        case protocol::ClientOpcode::CLAN_KICK: {
            push_string(nick);
            break;
        }

        case protocol::ClientOpcode::MEDITATE:
        case protocol::ClientOpcode::RESURRECT:
        case protocol::ClientOpcode::HEAL:
        case protocol::ClientOpcode::PICK_ITEM:
        case protocol::ClientOpcode::CLAN_REVIEW:
        case protocol::ClientOpcode::CLAN_LEAVE:
        case protocol::ClientOpcode::DISCONNECT: {
            break;
        }
    }

    uint16_t net_payload_size =
        htons(static_cast<uint16_t>(payload.size()));

    socket.sendall(&opcode, sizeof(opcode));
    socket.sendall(&net_payload_size, sizeof(net_payload_size));

    if (!payload.empty()) {
        socket.sendall(payload.data(), payload.size());
    }
}

Command Command::login(const std::string& nick) {
    Command cmd(0, protocol::ClientOpcode::LOGIN);
    cmd.nick = nick;
    return cmd;
}

Command Command::create_character(
    const std::string& nick,
    const std::string& raza,
    const std::string& clase) {

    Command cmd(0, protocol::ClientOpcode::CREATE_CHARACTER);
    cmd.nick = nick;
    cmd.raza = raza;
    cmd.clase = clase;
    return cmd;
}

Command Command::move(uint8_t direction) {
    Command cmd(0, protocol::ClientOpcode::MOVE);
    cmd.direction = direction;
    return cmd;
}

Command Command::attack(const std::string& nick) {
    Command cmd(0, protocol::ClientOpcode::ATTACK);
    cmd.nick = nick;
    return cmd;
}

Command Command::disconnect() {
    return Command(0, protocol::ClientOpcode::DISCONNECT);
}

bool Command::is_disconnect() const {
    return type == protocol::ClientOpcode::DISCONNECT;
}

protocol::ClientOpcode Command::get_type() const {
    return type;
}

uint16_t Command::get_player_id() const {
    return player_id;
}

uint8_t Command::get_direction() const {
    return direction;
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