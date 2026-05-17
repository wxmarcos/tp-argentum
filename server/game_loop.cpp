#include "game_loop.h"

#include <iostream>

void GameLoop::run() {

    std::cout << "[GameLoop] iniciado\n";

    while (should_keep_running()) {

        try {

            Command cmd =
                commands_queue.pop();

            std::cout
                << "[GameLoop] comando: "
                << cmd.text()
                << "\n";

            if (cmd.is_shutdown()) {
                break;
            }
            // Aquí se procesaría el comando y se actualizaría el estado del mundo.
            // update_world(cmd);

            Snapshot snapshot("[Snapshot]: " + cmd.text() + "\n");
            broadcast_snapshot(snapshot);

        } catch (const ClosedQueue&) {

            break;
        }
    }

    std::cout << "[GameLoop] finalizado\n";
}

void GameLoop::broadcast_snapshot(const Snapshot& snapshot) {
    for (auto& client : clients) {
        if (client && client->is_alive()) {
            client->push(snapshot);
        }
    }
}