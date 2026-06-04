#include <iostream>
#include <stdexcept>
#include <csignal>
#include <atomic>

#include "server.h"
#include "client/quit_listener.h"
#include "config/server_config.h"
#include "game/config.h"

static std::atomic<bool> running(true);

void handle_sigint(int) {
    running = false;
}

int main(int argc, char const *argv[]) {
    std::signal(SIGINT, handle_sigint);

    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <puerto>\n";
        return 1;
    }

    try {
        ServerConfig::load("config.toml");

        std::string port = argv[1];
        Config game_config("config.toml");

        Server server(port.c_str(), game_config);

        QuitListener quit_listener(running);
        quit_listener.start();

        server.run(running);

        quit_listener.stop();
        quit_listener.join();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}