#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <cstdint>
#include <memory>
#include <sys/socket.h>

#include "common/thread.h"
#include "common/network/socket.h"
#include "common/queue.h"
#include "common/command/command.h"

#include "client/monitor_clients.h"

class Acceptor: public Thread {

private:

    Socket listener;

    MonitorClients& clients;

    Queue<Command>& commands_queue;

    uint16_t next_player_id = 1;

    void close_listener();

    void stop_clients();

public:

    Acceptor(
    const char* port,
    MonitorClients& clients,
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