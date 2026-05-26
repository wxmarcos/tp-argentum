#include "game_loop.h"

#include <iostream>

void GameLoop::run() {

    std::cout << "[GameLoop] iniciado\n";

    while (should_keep_running()) {

        try {

            Command cmd = commands_queue.pop();

            if (cmd.is_disconnect()) {
                std::cout
                    << "[Receiver] Player "
                    << cmd.get_player_id()
                    << " disconnected\n";
                break;
            }

            switch (cmd.get_type()) {

                case CommandType::Move:
                    std::cout
                        << "[Receiver] MOVE dir="
                        << cmd.get_direction()
                        << "\n";
                    break;

                case CommandType::Attack:
                    std::cout
                        << "[Receiver] ATTACK target="
                        << cmd.get_target()
                        << "\n";
                    break;
                case CommandType::Disconnect:
                    std::cout
                        << "[Receiver] Player "
                        << cmd.get_player_id()
                        << " disconnected\n";
                    break;
            }

        } catch (const ClosedQueue&) {
            break;
        }
    }

    std::cout << "[GameLoop] finalizado\n";
}