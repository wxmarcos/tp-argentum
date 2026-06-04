#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include <memory>
#include <vector>

#include "game/game.h"

#include "common/thread.h"
#include "common/queue.h"
#include "common/command/command.h"
#include "common/snapshot/snapshot.h"

#include "client/monitor_clients.h"

#include "persistence/persistence_task.h"

class GameLoop : public Thread {

private:
    Queue<Command>& commands_queue;
    Queue<PersistenceTask>& persistence_queue;

    MonitorClients& clients;

    Config& config;

    Game game;

    void enqueue_persistence_tasks(const Command& cmd);
    void debug_snapshot(const Snapshot& snapshot) const;

public:
    GameLoop(
        Queue<Command>& commands_queue,
        Queue<PersistenceTask>& persistence_queue,
        MonitorClients& clients,
        Config& config);

    void run() override;

    void broadcast_snapshot(const Snapshot& snapshot);
};

#endif