#ifndef CLIENT_CLIENT_RECEIVER_H
#define CLIENT_CLIENT_RECEIVER_H

#include <string>

#include "common/network/socket.h"
#include "common/queue.h"
#include "common/thread.h"

#include "protocol/game_update.h"

class ClientReceiver: public Thread {
    private:
    Socket& socket;
    Queue<GameUpdate>& updates_queue;

    void feed(std::string& acc, char c);

    void push_update(GameUpdate update);

    void push_disconnect();

    public:
    ClientReceiver(Socket& socket, Queue<GameUpdate>& updates_queue);

    void run() override;
};

#endif