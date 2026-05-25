#include "receiver.h"
#include <iostream>

Receiver::Receiver(Socket& socket, Queue<Command>& queue)
    : socket(socket), queue(queue) {}

Receiver::~Receiver() = default;

void Receiver::run() {
    try {
        std::cout << "[Receiver] Iniciado\n";

        std::string acc;
        char buffer[1024];

        while (should_keep_running()) {
            int received = socket.recvsome(buffer, sizeof(buffer));
            if (received == 0) {
                std::cout << "[Receiver] Conexión cerrada por cliente\n";
                queue.push(Command("__DISCONNECT__"));
                break;
            }

            if (received < 0) {
                continue;
            }

            acc.append(buffer, buffer + received);

            std::size_t pos;
            while ((pos = acc.find('\n')) != std::string::npos) {
                std::string line = acc.substr(0, pos);
                acc.erase(0, pos + 1);

                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }

                if (!line.empty()) {
                    try {
                        queue.push(Command(std::move(line)));
                    } catch (const ClosedQueue&) {
                        return;
                    }
                }
            }
        }
    } catch (const std::exception& ex) {
        std::cerr << "[Receiver] Error: " << ex.what() << "\n";
        try {
            queue.push(Command("__DISCONNECT__"));
        } catch (const ClosedQueue&) {
        }
    }
}
