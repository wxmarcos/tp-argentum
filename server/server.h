#ifndef SERVER_H
#define SERVER_H

#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "common/command.h"
#include "common/queue.h"

#include "acceptor/acceptor.h"
#include "client/client_handler.h"
#include "config/server_config.h"
#include "game/game_loop.h"
#include "game/config.h"

#include "persistence/persistence_task.h"
#include "persistence/persistence_worker.h"

class Server {

private:
    std::vector<std::unique_ptr<ClientHandler>> clients;

    Queue<Command> commands_queue;
    Queue<PersistenceTask> persistence_queue;

    Acceptor acceptor;
    GameLoop gameloop;
    PersistenceWorker persistence_worker;

public:
    Server(const char* port, Config& game_config)
        :
        commands_queue(),
        persistence_queue(),

        acceptor(
            port,
            clients,
            commands_queue),

        gameloop(
            commands_queue,
            persistence_queue,
            clients,
            game_config),

        persistence_worker(
            persistence_queue,
            "saves") {}

    void start() {
        persistence_worker.start();
        acceptor.start();
        gameloop.start();
    }

    void stop() {
        gameloop.stop();

        commands_queue.close();
        persistence_queue.close();

        acceptor.stop();
    }

    void join() {
        gameloop.join();
        acceptor.join();
        persistence_worker.join();
    }

    void run(std::atomic<bool>& running) {
        start();

        std::cout << "[Server] Servidor corriendo.\n";

        while (running) {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(100));
        }

        std::cout << "[Server] Deteniendo servidor...\n";

        stop();
        join();

        std::cout
            << "[Server] Servidor detenido correctamente.\n";
    }
};

#endif