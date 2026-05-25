#ifndef COMMON_RECEIVER_H
#define COMMON_RECEIVER_H

#include "thread.h"
#include "network/socket.h"
#include "queue.h"
#include "command.h"

class Receiver: public Thread {
    private:
    Socket& socket;
    Queue<Command>& queue;

    public:
    Receiver(Socket& socket, Queue<Command>& queue);
    ~Receiver() override;

    void run() override;
};

#endif
