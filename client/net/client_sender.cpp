#include "net/client_sender.h"

#include <iostream>
#include <string>

#include "protocol/protocol.h"

ClientSender::ClientSender(Socket& socket, Queue<ClientCommand>& commands_queue):
        socket(socket), commands_queue(commands_queue) {}

void ClientSender::run() {
    try {
        while (should_keep_running()) {
            ClientCommand cmd = commands_queue.pop();
            send_command(cmd);
        }
    } catch (const ClosedQueue&) {
    } catch (const std::exception& ex) {
        std::cerr << "[ClientSender] " << ex.what() << "\n";
    }
}

void ClientSender::send_command(const ClientCommand& cmd) {
    std::string bytes = Protocol::serialize(cmd);
    if (bytes.empty()) {
        return;
    }
    socket.sendall(bytes.c_str(), static_cast<unsigned int>(bytes.size()));
}
