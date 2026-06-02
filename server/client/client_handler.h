#ifndef CLIENT_HANDLER_H_
#define CLIENT_HANDLER_H_

#include <memory>
#include <cstdint>

#include "common/thread.h"
#include "common/network/socket.h"
#include "common/queue.h"
#include "common/command.h"
#include "common/snapshot.h"
#include "network/receiver.h"
#include "common/sender.h"

class ClientHandler: public Thread {

private:

    uint16_t player_id;

    Socket client;

    Queue<Command>& commands_queue;

    Queue<Snapshot> sender_queue;

    std::unique_ptr<Receiver> receiver;

    std::unique_ptr<Sender<Snapshot>> sender;

public:

    ClientHandler(
        uint16_t player_id,
        Socket client,
        Queue<Command>& commands_queue);

    ~ClientHandler() override;

    void push(Snapshot element);

    void stop() override;

    void run() override;
};

#endif