#ifndef CLIENT_CLIENT_SENDER_H
#define CLIENT_CLIENT_SENDER_H

#include "common/network/socket.h"
#include "common/queue.h"
#include "common/thread.h"

#include "protocol/client_command.h"

class ClientSender: public Thread {
    private:
    Socket& socket;
    Queue<ClientCommand>& commands_queue;

    void send_command(const ClientCommand& cmd);

    public:
    ClientSender(Socket& socket, Queue<ClientCommand>& commands_queue);

    void run() override;
};

#endif