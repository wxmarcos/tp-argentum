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
#include "config.h"
#include "game_loop.h"

class Server {

private:

    std::vector<std::unique_ptr<ClientHandler>> clients;

    Queue<Command> commands_queue;

    Acceptor acceptor;

    GameLoop gameloop;

public:

    explicit Server(const char* port)
        :
        acceptor(
            port,
            clients,
            commands_queue),

        gameloop(
            commands_queue,
            clients,
            Config::MAP_WIDTH,
            Config::MAP_HEIGHT) {}

    void start() {

        acceptor.start();

        gameloop.start();
    }

    void stop() {

        acceptor.stop();

        commands_queue.close();

        gameloop.stop();
    }

    void join() {

        acceptor.join();

        gameloop.join();
    }

    void run() {

        start();

        std::cout
            << "Servidor corriendo.\n";

        std::string line;

        while (std::getline(std::cin, line)) {

            if (line == "q" ||
                line == "quit" ||
                line == "exit") {
                break;
            }
        }

        std::cout
            << "Deteniendo servidor...\n";

        stop();

        join();
    }
};

#endif