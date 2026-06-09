#ifndef CLIENT_HANDLER_H_
#define CLIENT_HANDLER_H_

#include <cstdint>
#include <memory>

#include "common/command/command.h"
#include "common/network/socket.h"
#include "common/queue.h"
#include "common/sender.h"
#include "common/snapshot/snapshot.h"
#include "common/thread.h"
#include "network/receiver.h"

class ClientHandler: public Thread {
private:
    uint16_t player_id;

    Socket client;

    Queue<Command>& commands_queue;

    Queue<Snapshot> sender_queue;

    std::unique_ptr<Receiver> receiver;

    std::unique_ptr<Sender<Snapshot>> sender;

public:
    ClientHandler(uint16_t player_id, Socket client,
                  Queue<Command>& commands_queue);

    ~ClientHandler() override;

    void push(Snapshot element);

    void stop() override;

    void run() override;
};

#endif
