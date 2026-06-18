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
                    debug_snapshot(out);
                    dispatch_snapshot(out);
                }
            }

        } catch (const ClosedQueue&) {
            break;
        }

        std::vector<OutgoingSnapshot> tick_snapshots = game.tick(dt);

        for (const OutgoingSnapshot& out : tick_snapshots) {
            debug_snapshot(out);
            dispatch_snapshot(out);
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
        persistence_queue.push(
            PersistenceJob::clans_job(game.getClanes()));
    }
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

static const char* delivery_to_string(DeliveryType delivery) {
    switch (delivery) {
        case DeliveryType::BROADCAST:
            return "BROADCAST";
        case DeliveryType::UNICAST:
            return "UNICAST";
        case DeliveryType::MULTICAST:
            return "MULTICAST";
        default:
            return "UNKNOWN";
    }
}

void GameLoop::debug_snapshot(const OutgoingSnapshot& out) const {
    const Snapshot& snapshot = out.snapshot;

    std::cout << "[GameLoop] "
              << delivery_to_string(out.delivery);

    if (!out.recipients.empty()) {
        std::cout << " recipients=[";
        for (size_t i = 0; i < out.recipients.size(); ++i) {
            if (i > 0) std::cout << ",";
            std::cout << out.recipients[i];
        }
        std::cout << "]";
    }

    std::cout << " ";

    if (snapshot.is_entity_created()) {
        std::cout << "ENTITY_CREATED "
                  << "nick=" << snapshot.get_nick()
                  << " mapa_id=" << snapshot.get_mapa_id()
                  << " x=" << snapshot.get_x()
                  << " y=" << snapshot.get_y()
                  << " dir=" << static_cast<int>(snapshot.get_direction());

    } else if (snapshot.is_entity_login()) {
        std::cout << "ENTITY_LOGIN "
                  << "nick=" << snapshot.get_nick()
                  << " mapa_id=" << snapshot.get_mapa_id()
                  << " x=" << snapshot.get_x()
                  << " y=" << snapshot.get_y()
                  << " dir=" << static_cast<int>(snapshot.get_direction());

    } else if (snapshot.is_entity_move()) {
        std::cout << "ENTITY_MOVE "
                  << "nick=" << snapshot.get_nick()
                  << " mapa_id=" << snapshot.get_mapa_id()
                  << " x=" << snapshot.get_x()
                  << " y=" << snapshot.get_y()
                  << " dir=" << static_cast<int>(snapshot.get_direction());

    } else if (snapshot.is_entity_remove()) {
        std::cout << "ENTITY_REMOVE "
                  << "nick=" << snapshot.get_nick();

    } else if (snapshot.is_damage_event()) {
        std::cout << "DAMAGE_EVENT "
                  << snapshot.get_attacker()
                  << " -> "
                  << snapshot.get_target()
                  << " damage=" << snapshot.get_damage()
                  << " critical=" << snapshot.is_critical();

    } else if (snapshot.is_dodge_event()) {
        std::cout << "DODGE_EVENT "
                  << snapshot.get_attacker()
                  << " -> "
                  << snapshot.get_target();

    } else if (snapshot.is_death_event()) {
        std::cout << "DEATH_EVENT "
                  << "target=" << snapshot.get_target();

    } else if (snapshot.is_error_message()) {
        std::cout << "ERROR_MESSAGE "
                  << "nick=" << snapshot.get_nick()
                  << " text=" << snapshot.get_text();

    } else if (snapshot.is_player_stats()) {
        std::cout << "PLAYER_STATS "
                  << "nick=" << snapshot.get_nick();

    } else if (snapshot.is_inventory_update()) {
        std::cout << "INVENTORY_UPDATE "
                  << "nick=" << snapshot.get_nick();

    } else if (snapshot.is_map_change()) {
        std::cout << "MAP_CHANGE "
                  << "nick=" << snapshot.get_nick()
                  << " mapa_id=" << snapshot.get_mapa_id()
                  << " x=" << snapshot.get_x()
                  << " y=" << snapshot.get_y();

    } else if (snapshot.is_cheat_status()) {
        std::cout << "CHEAT_STATUS "
                  << "nick=" << snapshot.get_nick();

    } else if (snapshot.is_chat_message()) {
        std::cout << "CHAT_MESSAGE "
                  << "from=" << snapshot.get_nick()
                  << " to=" << snapshot.get_target()
                  << " text=" << snapshot.get_text();

    } else if (snapshot.is_item_event()) {
        std::cout << "ITEM_EVENT "
                  << "nick=" << snapshot.get_nick()
                  << " item=" << snapshot.get_item_name()
                  << " amount=" << snapshot.get_amount()
                  << " mapa_id=" << snapshot.get_mapa_id()
                  << " x=" << snapshot.get_x()
                  << " y=" << snapshot.get_y();

    } else if (snapshot.is_meditation_status()) {
        std::cout << "MEDITATION_STATUS "
                  << "nick=" << snapshot.get_nick();

    } else {
        std::cout << "opcode="
                  << static_cast<int>(snapshot.get_opcode());
    }

    std::cout << "\n";
}