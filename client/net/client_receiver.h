#ifndef CLIENT_CLIENT_RECEIVER_H
#define CLIENT_CLIENT_RECEIVER_H

#include <cstdint>
#include <vector>

#include "common/network/socket.h"
#include "common/queue.h"
#include "common/thread.h"

#include "protocol/game_update.h"

class ClientReceiver: public Thread {
    private:
    Socket& socket;
    Queue<GameUpdate>& updates_queue;

    bool read_message(uint8_t& opcode, std::vector<uint8_t>& payload);
    void process_message(uint8_t opcode, const std::vector<uint8_t>& payload);

    void push_update(GameUpdate update);
    void push_disconnect();

    public:
    ClientReceiver(Socket& socket, Queue<GameUpdate>& updates_queue);

    void run() override;
};

#endif