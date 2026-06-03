#include "net/client_receiver.h"

#include <iostream>
#include <utility>

ClientReceiver::ClientReceiver(Socket& socket,
                               Queue<GameUpdate>& updates_queue):
        socket(socket), updates_queue(updates_queue) {}

void ClientReceiver::run() {
    try {
        while (should_keep_running()) {
            Snapshot snapshot = Snapshot::recv(socket);

            GameUpdate update;
            update.snapshot = std::move(snapshot);
            push_update(std::move(update));
        }
    } catch (const std::exception& ex) {
        if (should_keep_running()) {
            std::cerr << "[ClientReceiver] " << ex.what() << "\n";
    }
    push_disconnect();
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
