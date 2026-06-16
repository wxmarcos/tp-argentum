#include "common/command/command.h"

#include <arpa/inet.h>

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "common/command/command_codec.h"
#include "common/network/socket.h"

Command::Command(uint16_t player_id, protocol::ClientOpcode type):
    player_id(player_id), type(type) {}

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

    std::cout << "[Command] opcode=" << static_cast<int>(opcode_raw)
              << " payload_size=" << payload_size << "\n";

    std::vector<uint8_t> payload(payload_size);

    if (payload_size > 0) {
        received = socket.recvall(payload.data(), payload_size);

        if (received == 0) {
            return Command(player_id, protocol::ClientOpcode::DISCONNECT);
        }
    }

    auto type = static_cast<protocol::ClientOpcode>(opcode_raw);
    return parse_command_payload(payload, type, player_id);
}

void Command::send(Socket& socket) const {
    uint8_t opcode = static_cast<uint8_t>(type);
    std::vector<uint8_t> payload = build_command_payload(*this);
    uint16_t net_payload_size = htons(static_cast<uint16_t>(payload.size()));

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

Command Command::create_character(const std::string& nick,
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

Command Command::buy_item(const std::string& itemName) {
    Command cmd(0, protocol::ClientOpcode::BUY_ITEM);
    cmd.text = itemName;
    return cmd;
}

Command Command::deposit_gold(uint32_t amount) {
    Command cmd(0, protocol::ClientOpcode::DEPOSIT_GOLD);
    cmd.amount = amount;
    return cmd;
}

Command Command::withdraw_gold(uint32_t amount) {
    Command cmd(0, protocol::ClientOpcode::WITHDRAW_GOLD);
    cmd.amount = amount;
    return cmd;
}

Command Command::meditate() {
    return Command(0, protocol::ClientOpcode::MEDITATE);
}

Command Command::resurrect() {
    return Command(0, protocol::ClientOpcode::RESURRECT);
}

Command Command::heal() { return Command(0, protocol::ClientOpcode::HEAL); }

Command Command::pick_item() {
    return Command(0, protocol::ClientOpcode::PICK_ITEM);
}

Command Command::drop_item(uint16_t slot) {
    Command cmd(0, protocol::ClientOpcode::DROP_ITEM);
    cmd.slot = slot;
    return cmd;
}

Command Command::equip_item(uint16_t slot) {
    Command cmd(0, protocol::ClientOpcode::EQUIP_ITEM);
    cmd.slot = slot;
    return cmd;
}

Command Command::sell_item(uint16_t slot) {
    Command cmd(0, protocol::ClientOpcode::SELL_ITEM);
    cmd.slot = slot;
    return cmd;
}

Command Command::deposit_item(uint16_t slot) {
    Command cmd(0, protocol::ClientOpcode::DEPOSIT_ITEM);
    cmd.slot = slot;
    return cmd;
}

Command Command::withdraw_item(uint16_t item_id) {
    Command cmd(0, protocol::ClientOpcode::WITHDRAW_ITEM);
    cmd.item_id = item_id;
    return cmd;
}

Command Command::private_message(const std::string& nick,
                                 const std::string& text) {
    Command cmd(0, protocol::ClientOpcode::PRIVATE_MESSAGE);
    cmd.nick = nick;
    cmd.text = text;
    return cmd;
}

Command Command::cheat_god() {
    return Command(0, protocol::ClientOpcode::CHEAT_GOD);
}

Command Command::cheat_mana() {
    return Command(0, protocol::ClientOpcode::CHEAT_MANA);
}

Command Command::cheat_die() {
    return Command(0, protocol::ClientOpcode::CHEAT_DIE);
}

Command Command::cheat_resurrect() {
    return Command(0, protocol::ClientOpcode::CHEAT_RESURRECT);
}

Command Command::disconnect() {
    return Command(0, protocol::ClientOpcode::DISCONNECT);
}

bool Command::is_disconnect() const {
    return type == protocol::ClientOpcode::DISCONNECT;
}

protocol::ClientOpcode Command::get_type() const { return type; }

uint16_t Command::get_player_id() const { return player_id; }

uint8_t Command::get_direction() const { return direction; }

uint16_t Command::get_item_id() const { return item_id; }

uint16_t Command::get_slot() const { return slot; }

uint32_t Command::get_amount() const { return amount; }

const std::string& Command::get_nick() const { return nick; }

const std::string& Command::get_text() const { return text; }

const std::string& Command::get_clan_name() const { return clan_name; }

const std::string& Command::get_raza() const { return raza; }

const std::string& Command::get_clase() const { return clase; }
