#include "game/game_loop.h"

#include <chrono>
#include <iostream>
#include <thread>

GameLoop::GameLoop(Queue<Command>& commands_queue,
                   Queue<PersistenceTask>& persistence_queue,
                   MonitorClients& clients, Config& config):
    commands_queue(commands_queue),
    persistence_queue(persistence_queue), clients(clients), config(config),
    game(config) {}

void GameLoop::run() {
    std::cout << "[GameLoop] iniciado\n";

    const int ticks_per_second = config.getServerTicksPerSecond();

    const auto tick_duration =
        std::chrono::milliseconds(1000 / ticks_per_second);

    const float dt = 1.0f / static_cast<float>(ticks_per_second);

    while (should_keep_running()) {
        auto tick_start = std::chrono::steady_clock::now();

        try {
            Command cmd;

            while (commands_queue.try_pop(cmd)) {
                std::cout << "[GameLoop] Comando recibido: player_id="
                          << cmd.get_player_id()
                          << " type=" << static_cast<int>(cmd.get_type())
                          << "\n";

                std::vector<Snapshot> snapshots = game.process(cmd);

                enqueue_persistence_tasks(cmd);

                for (const Snapshot& snapshot : snapshots) {
                    broadcast_snapshot(snapshot);
                }
            }

        } catch (const ClosedQueue&) {
            break;
        }

        std::vector<Snapshot> tick_snapshots = game.tick(dt);

        for (const Snapshot& snapshot : tick_snapshots) {
            broadcast_snapshot(snapshot);
        }

        auto tick_end = std::chrono::steady_clock::now();
        auto elapsed = tick_end - tick_start;

        if (elapsed < tick_duration) {
            std::this_thread::sleep_for(tick_duration - elapsed);
        }
    }

    std::cout << "[GameLoop] finalizado\n";
}

void GameLoop::enqueue_persistence_tasks(const Command& cmd) {
    std::vector<PersistenceTask> tasks =
        game.build_persistence_tasks_for_command(cmd);

    for (const PersistenceTask& task : tasks) {
        persistence_queue.push(task);
    }
}

void GameLoop::debug_snapshot(const Snapshot& snapshot) const {
    if (snapshot.is_entity_created()) {
        std::cout << "[GameLoop] Broadcast ENTITY_CREATED "
                  << "nick=" << snapshot.get_nick() << " x=" << snapshot.get_x()
                  << " y=" << snapshot.get_y()
                  << " dir=" << static_cast<int>(snapshot.get_direction())
                  << "\n";

    } else if (snapshot.is_entity_login()) {
        std::cout << "[GameLoop] Broadcast ENTITY_LOGIN "
                  << "nick=" << snapshot.get_nick() << " x=" << snapshot.get_x()
                  << " y=" << snapshot.get_y()
                  << " dir=" << static_cast<int>(snapshot.get_direction())
                  << "\n";

    } else if (snapshot.is_entity_move()) {
        std::cout << "[GameLoop] Broadcast ENTITY_MOVE "
                  << "nick=" << snapshot.get_nick() << " x=" << snapshot.get_x()
                  << " y=" << snapshot.get_y()
                  << " dir=" << static_cast<int>(snapshot.get_direction())
                  << "\n";

    } else if (snapshot.is_entity_remove()) {
        std::cout << "[GameLoop] Broadcast ENTITY_REMOVE "
                  << "nick=" << snapshot.get_nick() << "\n";

    } else if (snapshot.is_damage_event()) {
        std::cout << "[GameLoop] Broadcast DAMAGE_EVENT "
                  << snapshot.get_attacker() << " -> " << snapshot.get_target()
                  << " damage=" << snapshot.get_damage()
                  << " critical=" << snapshot.is_critical() << "\n";

    } else if (snapshot.is_dodge_event()) {
        std::cout << "[GameLoop] Broadcast DODGE_EVENT "
                  << snapshot.get_attacker() << " -> " << snapshot.get_target()
                  << "\n";

    } else if (snapshot.is_death_event()) {
        std::cout << "[GameLoop] Broadcast DEATH_EVENT "
                  << "target=" << snapshot.get_target() << "\n";

    } else {
        std::cout << "[GameLoop] Broadcast opcode="
                  << static_cast<int>(snapshot.get_opcode()) << "\n";
    }
}

void GameLoop::broadcast_snapshot(const Snapshot& snapshot) {
    debug_snapshot(snapshot);
    clients.broadcast(snapshot);
}
