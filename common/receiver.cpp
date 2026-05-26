#include "receiver.h"
#include <iostream>

Receiver::Receiver(Socket& socket,
                   Queue<Command>& queue,
                   uint16_t player_id)
    : socket(socket),
      queue(queue),
      player_id(player_id)
{}

Receiver::~Receiver() = default;

void Receiver::run() {
    try {
        while (should_keep_running()) {
            Command cmd = Command::recv(socket);
            queue.push(std::move(cmd));
        }
    } catch (...) {}
}