#ifndef CLIENT_CLIENT_SENDER_H
#define CLIENT_CLIENT_SENDER_H

#include "common/command.h"
#include "common/network/socket.h"
#include "common/queue.h"
#include "common/thread.h"

class ClientSender: public Thread {
    private:
    Socket& socket;
    Queue<Command>& commands_queue;

    public:
    ClientSender(Socket& socket, Queue<Command>& commands_queue);

    void run() override;
};

#endif