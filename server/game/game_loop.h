#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include <memory>
#include <vector>

#include "game/game.h"

#include "common/thread.h"
#include "common/queue.h"
#include "common/command.h"
#include "common/snapshot.h"

#include "client/client_handler.h"

#include "persistence/persistence_task.h"

class GameLoop : public Thread {

private:
    Queue<Command>& commands_queue;
    Queue<PersistenceTask>& persistence_queue;

    std::vector<std::unique_ptr<ClientHandler>>& clients;

    Game game;

    void enqueue_persistence_tasks(const Command& cmd);
    void debug_snapshot(const Snapshot& snapshot) const;

public:
    GameLoop(
        Queue<Command>& commands_queue,
        Queue<PersistenceTask>& persistence_queue,
        std::vector<std::unique_ptr<ClientHandler>>& clients,
        Config& config);

    void run() override;

    void broadcast_snapshot(const Snapshot& snapshot);
};

#endif