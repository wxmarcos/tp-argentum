#include "net/server_connection.h"

#include <exception>
#include <utility>

namespace {
template <typename Action>
void quiet(Action&& action) {
    try {
        action();
    } catch (const std::exception&) {
    }
}
}  // namespace

ServerConnection::ServerConnection(const std::string& hostname,
                                   const std::string& servname):
    socket(hostname.c_str(), servname.c_str()),
    sender(socket, commands_queue), receiver(socket, updates_queue),
    running(true) {
    sender.start();
    receiver.start();
}

ServerConnection::~ServerConnection() { stop(); }

void ServerConnection::send(const Command& cmd) {
    if (!running) {
        return;
    }
    try {
        commands_queue.push(cmd);
    } catch (const ClosedQueue&) {
    }
}

bool ServerConnection::poll_update(GameUpdate& out) {
    if (!running) {
        return false;
    }
    try {
        return updates_queue.try_pop(out);
    } catch (const ClosedQueue&) {
        return false;
    }
}

void ServerConnection::stop() {
    if (!running) {
        return;
    }
    running = false;

    sender.stop();
    receiver.stop();

    quiet([&] { socket.shutdown(2); });
    quiet([&] { socket.close(); });

    quiet([&] { commands_queue.close(); });
    quiet([&] { updates_queue.close(); });

    quiet([&] { sender.join(); });
    quiet([&] { receiver.join(); });
}
