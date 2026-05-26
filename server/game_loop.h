#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include <vector>
#include <memory>
#include "world.h"
#include "common/thread.h"
#include "common/queue.h"
#include "common/command.h"
#include "common/snapshot.h"

#include "client_handler.h"

class GameLoop : public Thread {

private:
    
    Queue<Command>& commands_queue;

    std::vector<std::unique_ptr<ClientHandler>>& clients;
    void process(const Command& cmd);
    World world;
public:

    GameLoop(
        Queue<Command>& commands_queue,
        std::vector<std::unique_ptr<ClientHandler>>& clients)
        :
        commands_queue(commands_queue),
        clients(clients),
        world() {}

    void run() override;
    void broadcast_snapshot(const Snapshot& snapshot);
};

#endif