#ifndef CLIENT_CLIENT_RECEIVER_H
#define CLIENT_CLIENT_RECEIVER_H

#include "common/network/socket.h"
#include "common/queue.h"
#include "common/snapshot/snapshot.h"
#include "common/thread.h"
#include "protocol/game_update.h"

class ClientReceiver: public Thread {
private:
    Socket& socket;
    Queue<GameUpdate>& updates_queue;

    void push_update(GameUpdate update);
    void push_disconnect();

public:
    ClientReceiver(Socket& socket, Queue<GameUpdate>& updates_queue);

    void run() override;
};

#endif
