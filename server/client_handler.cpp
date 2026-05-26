#include "client_handler.h"

#include <iostream>

ClientHandler::ClientHandler(
    uint16_t player_id,
    Socket client,
    Queue<Command>& commands_queue):
    player_id(player_id),
    client(std::move(client)),
    commands_queue(commands_queue),
    receiver(std::make_unique<Receiver>(
        this->client,
        this->commands_queue,
        this->player_id)),
    sender(std::make_unique<Sender<Snapshot>>(
        this->client,
        sender_queue)) {}

ClientHandler::~ClientHandler() = default;

void ClientHandler::run() {

    try {

        std::cout
            << "[ClientHandler "
            << player_id
            << "] Iniciado\n";

        receiver->start();

        sender->start();

        receiver->join();

        std::cout
            << "[ClientHandler "
            << player_id
            << "] Receiver terminado\n";

        sender_queue.close();

        sender->join();

        std::cout
            << "[ClientHandler "
            << player_id
            << "] Finalizado\n";

    } catch (const std::exception& ex) {

        std::cerr
            << "[ClientHandler "
            << player_id
            << "] Error: "
            << ex.what()
            << "\n";
    }
}

void ClientHandler::push(
    Snapshot element) {

    sender_queue.push(
        std::move(element));
}