#include "acceptor.h"

#include <iostream>
#include <memory>
#include <thread>

void Acceptor::run() {

    try {

        while (should_keep_running()) {

            try {

                Socket client = listener.accept();

                uint16_t player_id =
                    next_player_id++;

                std::cout
                    << "[Acceptor] Cliente conectado. ID: "
                    << player_id
                    << "\n";

                auto handler =
                    std::make_unique<ClientHandler>(
                        player_id,
                        std::move(client),
                        commands_queue);

                handler->start();

                    clients.add(std::move(handler));

            } catch (const std::exception& ex) {

                if (should_keep_running()) {

                    std::cerr
                        << "Error aceptando cliente: "
                        << ex.what()
                        << "\n";
                }
            }
        }

    } catch (const std::exception& ex) {

        std::cerr
            << "Error en el hilo de aceptación: "
            << ex.what()
            << "\n";
    }

    std::cout << "[Acceptor] finalizado\n";
}

void Acceptor::close_listener() {

    try {
        listener.shutdown(SHUT_RDWR);
    } catch (...) {
    }

    try {
        listener.close();
    } catch (...) {
    }
}

void Acceptor::stop_clients() {
    clients.stop_all();
    clients.join_all();
    clients.clear();
}

void Acceptor::stop() {

    Thread::stop();

    close_listener();

    stop_clients();
}

Acceptor::~Acceptor() {

    close_listener();

    stop_clients();
}