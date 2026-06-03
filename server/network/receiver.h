#ifndef COMMON_RECEIVER_H
#define COMMON_RECEIVER_H

#include <cstdint>

#include "common/thread.h"
#include "common/network/socket.h"
#include "common/queue.h"
#include "common/command/command.h"

class Receiver: public Thread {

private:

    Socket& socket;

    Queue<Command>& queue;

    uint16_t player_id;

public:

    Receiver(Socket& socket,
             Queue<Command>& queue,
             uint16_t player_id);

    ~Receiver() override;

    void run() override;
};

#endif