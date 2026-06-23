#include "game/game_loop.h"

#include <chrono>
#include <iostream>
#include <thread>

GameLoop::GameLoop(Queue<Command>& commands_queue,
                   Queue<PersistenceJob>& persistence_queue,
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

    const float intervalo_persistencia = config.getPersistenciaIntervalo();
    float tiempo_desde_persist = 0.0f;

    while (should_keep_running()) {
        auto tick_start = std::chrono::steady_clock::now();

        try {
            Command cmd(0, protocol::ClientOpcode::DISCONNECT);

            while (commands_queue.try_pop(cmd)) {
                std::cout << "[GameLoop] Comando recibido: player_id="
                          << cmd.get_player_id()
                          << " type=" << static_cast<int>(cmd.get_type())
                          << "\n";

                std::vector<OutgoingSnapshot> snapshots;

                if (cmd.is_disconnect()) {
                    enqueue_persistence_tasks(cmd);
                    snapshots = game.process(cmd);
                } else {
                    snapshots = game.process(cmd);
                    enqueue_persistence_tasks(cmd);
                }

                for (const OutgoingSnapshot& out : snapshots) {
                    dispatch_snapshot(out);
                }
            }

        } catch (const ClosedQueue&) {
            break;
        }

        std::vector<OutgoingSnapshot> tick_snapshots = game.tick(dt);

        for (const OutgoingSnapshot& out : tick_snapshots) {
            dispatch_snapshot(out);
        }

        tiempo_desde_persist += dt;
        if (tiempo_desde_persist >= intervalo_persistencia) {
            enqueue_all_persistence_tasks();
            tiempo_desde_persist = 0.0f;
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
        persistence_queue.push(PersistenceJob::player_job(task));
    }

    if (game.command_changes_clans(cmd)) {
        persistence_queue.push(PersistenceJob::clans_job(game.getClanes()));
    }
}

void GameLoop::enqueue_all_persistence_tasks() {
    std::vector<PersistenceTask> tasks = game.build_all_players_tasks();

    for (const PersistenceTask& task : tasks) {
        persistence_queue.push(PersistenceJob::player_job(task));
    }

    persistence_queue.push(PersistenceJob::clans_job(game.getClanes()));
}

void GameLoop::dispatch_snapshot(const OutgoingSnapshot& out) {
    switch (out.delivery) {
        case DeliveryType::BROADCAST:
            broadcast_snapshot(out.snapshot);
            break;

        case DeliveryType::UNICAST:
            if (!out.recipients.empty()) {
                unicast_snapshot(out.recipients[0], out.snapshot);
            }
            break;

        case DeliveryType::MULTICAST:
            multicast_snapshot(out.recipients, out.snapshot);
            break;
    }
}

void GameLoop::broadcast_snapshot(const Snapshot& snapshot) {
    clients.broadcast(snapshot);
}

void GameLoop::unicast_snapshot(uint16_t player_id, const Snapshot& snapshot) {
    clients.send_to(player_id, snapshot);
}

void GameLoop::multicast_snapshot(const std::vector<uint16_t>& player_ids,
                                  const Snapshot& snapshot) {
    clients.send_to_many(player_ids, snapshot);
}
