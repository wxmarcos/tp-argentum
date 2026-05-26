#include "game_loop.h"

#include <iostream>
#include <chrono>
#include <thread>

static constexpr int TICKS_PER_SECOND = 30;
static constexpr auto TICK_DURATION =
    std::chrono::milliseconds(1000 / TICKS_PER_SECOND);

void GameLoop::run() {

    std::cout << "[GameLoop] iniciado\n";

    while (should_keep_running()) {

        auto tick_start = std::chrono::steady_clock::now();

        try {

            while (true) {
                Command cmd = commands_queue.pop();
                process(cmd);
            }

        } catch (const ClosedQueue&) {
            break;
        }

        world.update();

        Snapshot snapshot = world.build_snapshot();
        broadcast_snapshot(snapshot);

        auto tick_end = std::chrono::steady_clock::now();
        auto elapsed = tick_end - tick_start;

        if (elapsed < TICK_DURATION) {
            std::this_thread::sleep_for(TICK_DURATION - elapsed);
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
            world.process_command(cmd);
            break;

        case CommandType::Attack:
            std::cout
                << "[GameLoop] ATTACK target="
                << cmd.get_target()
                << "\n";
            world.process_command(cmd);
            break;

        case CommandType::Disconnect:
            std::cout
                << "[GameLoop] DISCONNECT player="
                << cmd.get_player_id()
                << "\n";
            break;
    }
}