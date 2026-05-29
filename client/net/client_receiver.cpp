#include "net/client_receiver.h"

#include <iostream>
#include <utility>

#include <arpa/inet.h>

#include "common/protocol_defs.h"

ClientReceiver::ClientReceiver(Socket& socket, Queue<GameUpdate>& updates_queue):
        socket(socket), updates_queue(updates_queue) {}

void ClientReceiver::run() {
    try {
        uint8_t opcode;
        std::vector<uint8_t> payload;

        while (should_keep_running()) {
            if (!read_message(opcode, payload)) {
                push_disconnect();
                break;
            }
            process_message(opcode, payload);
        }
    } catch (const std::exception& ex) {
        std::cerr << "[ClientReceiver] " << ex.what() << "\n";
        push_disconnect();
    }
}

bool ClientReceiver::read_message(uint8_t& opcode, std::vector<uint8_t>& payload) {
    int received = socket.recvall(&opcode, sizeof(opcode));
    if (received == 0) {
        return false;
    }

    uint16_t net_payload_size;
    received = socket.recvall(&net_payload_size, sizeof(net_payload_size));
    if (received == 0) {
        return false;
    }

    uint16_t payload_size = ntohs(net_payload_size);
    payload.resize(payload_size);

    if (payload_size > 0) {
        received = socket.recvall(payload.data(), payload_size);
        if (received == 0) {
            return false;
        }
    }
    return true;
}

void ClientReceiver::process_message(uint8_t opcode,
                                     const std::vector<uint8_t>& payload) {
    auto server_opcode = static_cast<protocol::ServerOpcode>(opcode);

    switch (server_opcode) {
        case protocol::ServerOpcode::SNAPSHOT:
            (void)payload;
            break;

        case protocol::ServerOpcode::ENTITY_SPAWN:
        case protocol::ServerOpcode::ENTITY_MOVE:
        case protocol::ServerOpcode::ENTITY_REMOVE:
        case protocol::ServerOpcode::PLAYER_STATS:
        case protocol::ServerOpcode::INVENTORY_UPDATE:
        case protocol::ServerOpcode::CHAT_MESSAGE:
        case protocol::ServerOpcode::DAMAGE_EVENT:
        case protocol::ServerOpcode::DODGE_EVENT:
        case protocol::ServerOpcode::DEATH_EVENT:
        case protocol::ServerOpcode::ERROR_MESSAGE:
            break;
    }
}

void ClientReceiver::push_update(GameUpdate update) {
    try {
        updates_queue.push(std::move(update));
    } catch (const ClosedQueue&) {
    }
}

void ClientReceiver::push_disconnect() {
    GameUpdate u;
    u.disconnect = true;
    push_update(std::move(u));
}