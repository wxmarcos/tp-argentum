#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <vector>
#include <memory>

#include "common/thread.h"
#include "common/network/socket.h"
#include "common/queue.h"
#include "common/command.h"

#include "client_handler.h"

class Acceptor: public Thread {

private:

    Socket listener;

    std::vector<std::unique_ptr<ClientHandler>>& clients;

    Queue<Command>& commands_queue;

    void close_listener();

public:

    Acceptor(
        const char* port,
        std::vector<std::unique_ptr<ClientHandler>>& clients,
        Queue<Command>& commands_queue)
        :
        listener(port),
        clients(clients),
        commands_queue(commands_queue) {}

    void run() override;
    void stop() override;

    ~Acceptor() override;
};

#endif