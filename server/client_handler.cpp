#include "client_handler.h"

#include <iostream>

ClientHandler::ClientHandler(Socket client, Queue<Command>& commands_queue):
    client(std::move(client)),
    commands_queue(commands_queue),
    receiver(std::make_unique<Receiver>(this->client,this->commands_queue)),
    sender(std::make_unique<Sender<Snapshot>>(this->client, sender_queue)) {}

ClientHandler::~ClientHandler() = default;

void ClientHandler::run() {
    try {

        std::cout << "[ClientHandler] Iniciado\n";

        receiver->start();
        sender->start();

        receiver->join();

        std::cout << "[ClientHandler] Receiver terminado\n";

        sender_queue.close();

        sender->join();

        std::cout << "[ClientHandler] Finalizado\n";

    } catch (const std::exception& ex) {

        std::cerr << "[ClientHandler] Error: "
                  << ex.what() << "\n";
    }
}
void ClientHandler::push(Snapshot element){
    sender_queue.push(std::move(element));
}