#include "game_loop.h"

#include <chrono>
#include <iostream>
#include <thread>

#include "server_config.h"

GameLoop::GameLoop(
    Queue<Command>& commands_queue,
    std::vector<std::unique_ptr<ClientHandler>>& clients,
    Config& config)
    : commands_queue(commands_queue),
      clients(clients),
      game(config) {}

void GameLoop::run() {

    std::cout << "[GameLoop] iniciado\n";

    const auto tick_duration =
        std::chrono::milliseconds(
            1000 / ServerConfig::TICKS_PER_SECOND);

    while (should_keep_running()) {

        auto tick_start =
            std::chrono::steady_clock::now();

        try {

            Command cmd =
                commands_queue.pop();
            std::cout << "[GameLoop] Comando recibido: player_id=" << cmd.get_player_id() << " type=" << static_cast<int>(cmd.get_type()) << "\n";
            
            std::vector<Snapshot> snapshots =
                game.process(cmd);

            for (const Snapshot& snapshot : snapshots) {
                broadcast_snapshot(snapshot);
            }

        } catch (const ClosedQueue&) {
            break;
        }

        // update lógica del juego
        float dt =
            std::chrono::duration<float>(
                tick_duration).count();

        game.tick(dt);

        auto tick_end =
            std::chrono::steady_clock::now();

        auto elapsed =
            tick_end - tick_start;

        if (elapsed < tick_duration) {
            std::this_thread::sleep_for(
                tick_duration - elapsed);
        }
    }

    std::cout << "[GameLoop] finalizado\n";
}

void GameLoop::broadcast_snapshot(const Snapshot& snapshot) {
    if (snapshot.is_entity_move()) {
        std::cout
            << "[GameLoop] Broadcast ENTITY_MOVE "
            << "nick=" << snapshot.get_nick()
            << " x=" << snapshot.get_x()
            << " y=" << snapshot.get_y()
            << " dir=" << static_cast<int>(snapshot.get_direction())
            << "\n";

    } else if (snapshot.is_entity_remove()) {
        std::cout
            << "[GameLoop] Broadcast ENTITY_REMOVE "
            << "nick=" << snapshot.get_nick()
            << "\n";

    } else if (snapshot.is_damage_event()) {
        std::cout
            << "[GameLoop] Broadcast DAMAGE_EVENT "
            << snapshot.get_attacker()
            << " -> "
            << snapshot.get_target()
            << " damage=" << snapshot.get_damage()
            << " critical=" << snapshot.is_critical()
            << "\n";

    } else if (snapshot.is_dodge_event()) {
        std::cout
            << "[GameLoop] Broadcast DODGE_EVENT "
            << snapshot.get_attacker()
            << " -> "
            << snapshot.get_target()
            << "\n";

    } else if (snapshot.is_death_event()) {
        std::cout
            << "[GameLoop] Broadcast DEATH_EVENT "
            << "target=" << snapshot.get_target()
            << "\n";

    } else {
        std::cout
            << "[GameLoop] Broadcast opcode="
            << static_cast<int>(snapshot.get_opcode())
            << "\n";
    }

    for (auto& client : clients) {
        if (client) {
            client->push(snapshot);
        }
    }
}