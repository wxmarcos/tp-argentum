#include "net/client_sender.h"

#include <iostream>

ClientSender::ClientSender(Socket& socket, Queue<Command>& commands_queue):
    socket(socket), commands_queue(commands_queue) {}

void ClientSender::run() {
    try {
        while (should_keep_running()) {
            Command cmd = commands_queue.pop();
            cmd.send(socket);
            if (cmd.is_disconnect()) {
                break;
            }
        }
    } catch (const ClosedQueue&) {
    } catch (const std::exception& ex) {
        std::cerr << "[ClientSender] " << ex.what() << "\n";
    }
}
