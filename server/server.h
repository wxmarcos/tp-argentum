#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "common/queue.h"
#include "common/command.h"

#include "client_handler.h"
#include "acceptor.h"
#include "game_loop.h"

class Server {

private:

    std::vector<std::unique_ptr<ClientHandler>> clients;

    Queue<Command> commands_queue;

    Acceptor acceptor;

    GameLoop gameloop;

public:

    Server(const char* port)
        :
        acceptor(
            port,
            clients,
            commands_queue),

        gameloop(
            commands_queue,
            clients) {}

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

        std::cout << "Servidor corriendo.\n";

        std::string line;
        while (std::getline(std::cin, line)) {
            if (line == "q" || line == "quit" || line == "exit") {
                break;
            }
        }

        std::cout << "Deteniendo servidor...\n";
        stop();
        join();
    }
};

#endif