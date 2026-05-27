#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "common/command.h"
#include "common/queue.h"

#include "acceptor.h"
#include "client_handler.h"
#include "server_config.h"
#include "game_loop.h"

#include "game/config.h"

class Server {

private:
    std::vector<std::unique_ptr<ClientHandler>> clients;
    Queue<Command> commands_queue;

    Acceptor acceptor;
    GameLoop gameloop;

public:
    Server(const char* port, Config& game_config)
        :
        acceptor(
            port,
            clients,
            commands_queue),

        gameloop(
            commands_queue,
            clients,
            game_config,
            ServerConfig::MAP_WIDTH,
            ServerConfig::MAP_HEIGHT) {}

    void start() {
        acceptor.start();
        gameloop.start();
    }

    void stop() {
        gameloop.stop();
        commands_queue.close();
        acceptor.stop();
    }

    void join() {
        gameloop.join();
        acceptor.join();
    }

    void run() {
        start();

        std::cout << "Servidor corriendo.\n";

        std::string line;

        while (std::getline(std::cin, line)) {
            if (line == "q" ||
                line == "quit" ||
                line == "exit") {
                break;
            }
        }

        std::cout << "Deteniendo servidor...\n";

        stop();
        join();
    }
};

#endif