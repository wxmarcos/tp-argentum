#include "game_loop.h"

#include <chrono>
#include <iostream>
#include <thread>

#include "config.h"

GameLoop::GameLoop(
    Queue<Command>& commands_queue,
    std::vector<std::unique_ptr<ClientHandler>>& clients,
    int anchoMapa,
    int altoMapa)
    : commands_queue(commands_queue),
      clients(clients),
      game(anchoMapa, altoMapa) {}

void GameLoop::run() {

    std::cout << "[GameLoop] iniciado\n";

    const auto tick_duration =
        std::chrono::milliseconds(
            1000 / Config::TICKS_PER_SECOND);

    while (should_keep_running()) {

        auto tick_start = std::chrono::steady_clock::now();

        try {

            Command cmd = commands_queue.pop();

            process(cmd);

        } catch (const ClosedQueue&) {
            break;
        }

        // update lógica del juego
        game.tick();

        // snapshot futuro
        // Snapshot snapshot = game.build_snapshot();
        // broadcast_snapshot(snapshot);

        auto tick_end = std::chrono::steady_clock::now();

        auto elapsed = tick_end - tick_start;

        if (elapsed < tick_duration) {
            std::this_thread::sleep_for(
                tick_duration - elapsed);
        }
    }

    std::cout << "[GameLoop] finalizado\n";
}

void GameLoop::process(const Command& cmd) {

    if (cmd.is_disconnect()) {

        std::cout
            << "[GameLoop] Player "
            << cmd.get_player_id()
            << " disconnected\n";

        return;
    }

    switch (cmd.get_type()) {

        case CommandType::Move:

            std::cout
                << "[GameLoop] MOVE dir="
                << cmd.get_direction()
                << "\n";

            /*
             * TODO:
             * mapear player_id -> nombre
             *
             * ejemplo futuro:
             *
             * game.moverJugador(
             *     nombre,
             *     cmd.get_direction());
             */

            break;

        case CommandType::Attack:

            std::cout
                << "[GameLoop] ATTACK target="
                << cmd.get_target()
                << "\n";

            break;

        case CommandType::Disconnect:

            std::cout
                << "[GameLoop] DISCONNECT player="
                << cmd.get_player_id()
                << "\n";

            break;
    }
}

void GameLoop::broadcast_snapshot(
    const Snapshot& snapshot) {

    for (auto& client : clients) {

        if (client) {
            client->push(snapshot);
        }
    }
}