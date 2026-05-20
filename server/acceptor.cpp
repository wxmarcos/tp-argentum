#include "acceptor.h"
#include <memory>
#include <iostream>
#include <thread>

void Acceptor::run() {
    try {

        while (should_keep_running()) {

            try {

                Socket client = listener.accept();

                std::cout << "[Acceptor] Cliente conectado\n";

                auto handler =
                    std::make_unique<ClientHandler>(
                        std::move(client),
                        commands_queue);

                handler->start();

                clients.push_back(std::move(handler));

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
}

void Acceptor::close_listener() {
    try {
        listener.close();
    } catch (...) {
    }
}

void Acceptor::stop() {
    close_listener();
    Thread::stop();
}

Acceptor::~Acceptor() {
    close_listener();

    for (auto& client : clients) {
        if (client && client->is_alive()) {
            client->stop();
        }
    }

    for (auto& client : clients) {
        if (client && client->is_alive()) {
            client->join();
        }
    }

    clients.clear();
}