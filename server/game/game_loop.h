#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include <memory>
#include <vector>

#include "client/monitor_clients.h"
#include "common/command/command.h"
#include "common/queue.h"
#include "common/snapshot/snapshot.h"
#include "common/snapshot/snapshot_outgoing.h"
#include "common/thread.h"
#include "game/game.h"
#include "persistence/persistence_job.h"

class GameLoop: public Thread {
private:
    Queue<Command>& commands_queue;
    Queue<PersistenceJob>& persistence_queue;

    MonitorClients& clients;

    Config& config;

    Game game;

    void enqueue_persistence_tasks(const Command& cmd);
    void dispatch_snapshot(const OutgoingSnapshot& out);

    void broadcast_snapshot(const Snapshot& snapshot);

    void unicast_snapshot(uint16_t player_id, const Snapshot& snapshot);

    void multicast_snapshot(const std::vector<uint16_t>& player_ids,
                            const Snapshot& snapshot);
    void debug_snapshot(const OutgoingSnapshot& out) const;

public:
    GameLoop(Queue<Command>& commands_queue,
             Queue<PersistenceJob>& persistence_queue, MonitorClients& clients,
             Config& config);

    void run() override;
};

#endif
