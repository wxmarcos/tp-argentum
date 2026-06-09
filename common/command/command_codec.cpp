#include "command_codec.h"

#include <stdexcept>

#include "packet_helpers.h"

Command parse_login(const std::vector<uint8_t>& payload, size_t& offset,
                    uint16_t player_id) {
    Command cmd(player_id, protocol::ClientOpcode::LOGIN);
    cmd.nick = read_string(payload, offset);
    return cmd;
}

Command parse_create_character(const std::vector<uint8_t>& payload,
                               size_t& offset, uint16_t player_id) {
    Command cmd(player_id, protocol::ClientOpcode::CREATE_CHARACTER);
    cmd.nick = read_string(payload, offset);
    cmd.raza = read_string(payload, offset);
    cmd.clase = read_string(payload, offset);
    return cmd;
}

Command parse_move(const std::vector<uint8_t>& payload, size_t& offset,
                   uint16_t player_id) {
    Command cmd(player_id, protocol::ClientOpcode::MOVE);
    cmd.direction = read_u8(payload, offset);
    return cmd;
}

Command parse_attack(const std::vector<uint8_t>& payload, size_t& offset,
                     uint16_t player_id) {
    Command cmd(player_id, protocol::ClientOpcode::ATTACK);
    cmd.nick = read_string(payload, offset);
    return cmd;
}

Command parse_item_id(const std::vector<uint8_t>& payload, size_t& offset,
                      protocol::ClientOpcode opcode, uint16_t player_id) {
    Command cmd(player_id, opcode);
    cmd.item_id = read_u16(payload, offset);
    return cmd;
}

Command parse_item_and_amount(const std::vector<uint8_t>& payload,
                              size_t& offset, protocol::ClientOpcode opcode,
                              uint16_t player_id) {
    Command cmd(player_id, opcode);
    cmd.item_id = read_u16(payload, offset);
    cmd.amount = read_u32(payload, offset);
    return cmd;
}

Command parse_text(const std::vector<uint8_t>& payload, size_t& offset,
                   protocol::ClientOpcode opcode, uint16_t player_id) {
    Command cmd(player_id, opcode);
    cmd.text = read_string(payload, offset);
    return cmd;
}

Command parse_amount(const std::vector<uint8_t>& payload, size_t& offset,
                     protocol::ClientOpcode opcode, uint16_t player_id) {
    Command cmd(player_id, opcode);
    cmd.amount = read_u32(payload, offset);
    return cmd;
}

Command parse_slot(const std::vector<uint8_t>& payload, size_t& offset,
                   protocol::ClientOpcode opcode, uint16_t player_id) {
    Command cmd(player_id, opcode);
    cmd.slot = read_u16(payload, offset);
    return cmd;
}

Command parse_slot_and_amount(const std::vector<uint8_t>& payload,
                              size_t& offset, protocol::ClientOpcode opcode,
                              uint16_t player_id) {
    Command cmd(player_id, opcode);
    cmd.slot = read_u16(payload, offset);
    cmd.amount = read_u32(payload, offset);
    return cmd;
}

Command parse_private_message(const std::vector<uint8_t>& payload,
                              size_t& offset, uint16_t player_id) {
    Command cmd(player_id, protocol::ClientOpcode::PRIVATE_MESSAGE);
    cmd.nick = read_string(payload, offset);
    cmd.text = read_string(payload, offset);
    return cmd;
}

Command parse_clan_name(const std::vector<uint8_t>& payload, size_t& offset,
                        protocol::ClientOpcode opcode, uint16_t player_id) {
    Command cmd(player_id, opcode);
    cmd.clan_name = read_string(payload, offset);
    return cmd;
}

Command parse_nick_only(const std::vector<uint8_t>& payload, size_t& offset,
                        protocol::ClientOpcode opcode, uint16_t player_id) {
    Command cmd(player_id, opcode);
    cmd.nick = read_string(payload, offset);
    return cmd;
}

Command parse_command_payload(const std::vector<uint8_t>& payload,
                              protocol::ClientOpcode type, uint16_t player_id) {
    size_t offset = 0;
    Command cmd(player_id, type);

    switch (type) {
        case protocol::ClientOpcode::LOGIN:
            cmd = parse_login(payload, offset, player_id);
            break;

        case protocol::ClientOpcode::CREATE_CHARACTER:
            cmd = parse_create_character(payload, offset, player_id);
            break;

        case protocol::ClientOpcode::MOVE:
            cmd = parse_move(payload, offset, player_id);
            break;

        case protocol::ClientOpcode::ATTACK:
            cmd = parse_attack(payload, offset, player_id);
            break;

        case protocol::ClientOpcode::DROP_ITEM:
            cmd = parse_slot(payload, offset, type, player_id);
            break;
        case protocol::ClientOpcode::EQUIP_ITEM:
            cmd = parse_slot(payload, offset, type, player_id);
            break;

        case protocol::ClientOpcode::BUY_ITEM:
            cmd = parse_text(payload, offset, type, player_id);
            break;

        case protocol::ClientOpcode::SELL_ITEM:
        case protocol::ClientOpcode::DEPOSIT_ITEM:
            cmd = parse_slot_and_amount(payload, offset, type, player_id);
            break;

        case protocol::ClientOpcode::WITHDRAW_ITEM:
            cmd = parse_item_and_amount(payload, offset, type, player_id);
            break;

        case protocol::ClientOpcode::DEPOSIT_GOLD:
        case protocol::ClientOpcode::WITHDRAW_GOLD:
            cmd = parse_amount(payload, offset, type, player_id);
            break;

        case protocol::ClientOpcode::PRIVATE_MESSAGE:
            cmd = parse_private_message(payload, offset, player_id);
            break;

        case protocol::ClientOpcode::CLAN_CREATE:
        case protocol::ClientOpcode::CLAN_JOIN:
            cmd = parse_clan_name(payload, offset, type, player_id);
            break;

        case protocol::ClientOpcode::CLAN_ACCEPT:
        case protocol::ClientOpcode::CLAN_REJECT:
        case protocol::ClientOpcode::CLAN_BAN:
        case protocol::ClientOpcode::CLAN_KICK:
            cmd = parse_nick_only(payload, offset, type, player_id);
            break;

        case protocol::ClientOpcode::MEDITATE:
        case protocol::ClientOpcode::RESURRECT:
        case protocol::ClientOpcode::HEAL:
        case protocol::ClientOpcode::PICK_ITEM:
        case protocol::ClientOpcode::CLAN_REVIEW:
        case protocol::ClientOpcode::CLAN_LEAVE:
        case protocol::ClientOpcode::DISCONNECT:
            cmd = Command(player_id, type);
            break;

        default:
            throw std::runtime_error("opcode desconocido");
    }

    validate_no_extra_bytes(payload, offset);
    return cmd;
}

static void serialize_login(const Command& command,
                            std::vector<uint8_t>& payload) {
    push_string(payload, command.get_nick());
}

static void serialize_create_character(const Command& command,
                                       std::vector<uint8_t>& payload) {
    push_string(payload, command.get_nick());
    push_string(payload, command.get_raza());
    push_string(payload, command.get_clase());
}

static void serialize_move(const Command& command,
                           std::vector<uint8_t>& payload) {
    push_u8(payload, command.get_direction());
}

static void serialize_attack(const Command& command,
                             std::vector<uint8_t>& payload) {
    push_string(payload, command.get_nick());
}

static void serialize_item_and_amount(const Command& command,
                                      std::vector<uint8_t>& payload) {
    push_u16(payload, command.get_item_id());
    push_u32(payload, command.get_amount());
}
static void serialize_slot(const Command& command,
                           std::vector<uint8_t>& payload) {
    push_u16(payload, command.get_slot());
}

static void serialize_slot_and_amount(const Command& command,
                                      std::vector<uint8_t>& payload) {
    push_u16(payload, command.get_slot());
    push_u32(payload, command.get_amount());
}

static void serialize_private_message(const Command& command,
                                      std::vector<uint8_t>& payload) {
    push_string(payload, command.get_nick());
    push_string(payload, command.get_text());
}

static void serialize_clan_name(const Command& command,
                                std::vector<uint8_t>& payload) {
    push_string(payload, command.get_clan_name());
}

static void serialize_nick_only(const Command& command,
                                std::vector<uint8_t>& payload) {
    push_string(payload, command.get_nick());
}

std::vector<uint8_t> build_command_payload(const Command& command) {
    std::vector<uint8_t> payload;

    switch (command.get_type()) {
        case protocol::ClientOpcode::LOGIN:
            serialize_login(command, payload);
            break;

        case protocol::ClientOpcode::CREATE_CHARACTER:
            serialize_create_character(command, payload);
            break;

        case protocol::ClientOpcode::MOVE:
            serialize_move(command, payload);
            break;

        case protocol::ClientOpcode::ATTACK:
            serialize_attack(command, payload);
            break;

        case protocol::ClientOpcode::DROP_ITEM:
        case protocol::ClientOpcode::EQUIP_ITEM:
            serialize_slot(command, payload);
            break;

        case protocol::ClientOpcode::BUY_ITEM:
            push_string(payload, command.get_text());
            break;

        case protocol::ClientOpcode::SELL_ITEM:
        case protocol::ClientOpcode::DEPOSIT_ITEM:
            serialize_slot_and_amount(command, payload);
            break;

        case protocol::ClientOpcode::WITHDRAW_ITEM:
            serialize_item_and_amount(command, payload);
            break;

        case protocol::ClientOpcode::DEPOSIT_GOLD:
        case protocol::ClientOpcode::WITHDRAW_GOLD:
            push_u32(payload, command.get_amount());
            break;

        case protocol::ClientOpcode::PRIVATE_MESSAGE:
            serialize_private_message(command, payload);
            break;

        case protocol::ClientOpcode::CLAN_CREATE:
        case protocol::ClientOpcode::CLAN_JOIN:
            serialize_clan_name(command, payload);
            break;

        case protocol::ClientOpcode::CLAN_ACCEPT:
        case protocol::ClientOpcode::CLAN_REJECT:
        case protocol::ClientOpcode::CLAN_BAN:
        case protocol::ClientOpcode::CLAN_KICK:
            serialize_nick_only(command, payload);
            break;

        case protocol::ClientOpcode::MEDITATE:
        case protocol::ClientOpcode::RESURRECT:
        case protocol::ClientOpcode::HEAL:
        case protocol::ClientOpcode::PICK_ITEM:
        case protocol::ClientOpcode::CLAN_REVIEW:
        case protocol::ClientOpcode::CLAN_LEAVE:
        case protocol::ClientOpcode::DISCONNECT:
            break;

        default:
            throw std::runtime_error("opcode desconocido");
    }

    return payload;
}
