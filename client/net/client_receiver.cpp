#include "net/client_receiver.h"

#include <iostream>
#include <string>
#include <utility>

#include "protocol/protocol.h"

ClientReceiver::ClientReceiver(Socket& socket, Queue<GameUpdate>& updates_queue):
        socket(socket), updates_queue(updates_queue) {}

void ClientReceiver::run() {
    try {
        std::string acc;
        char c;

        while (should_keep_running()) {
            int received = socket.recvsome(&c, 1);

            if (received == 0) {
                push_disconnect();
                break;
            }
            if (received < 0) {
                continue;
            }

            feed(acc, c);
        }
    } catch (const std::exception& ex) {
        std::cerr << "[ClientReceiver] " << ex.what() << "\n";
        push_disconnect();
    }
}

void ClientReceiver::feed(std::string& acc, char c) {
    if (c != '\n') {
        acc.push_back(c);
        return;
    }

    if (!acc.empty() && acc.back() == '\r') {
        acc.pop_back();
    }
    if (!acc.empty()) {
        push_update(Protocol::parse(acc));
    }
    acc.clear();
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
