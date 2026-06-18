#pragma once

#include <string>

#include "common/queue.h"
#include "common/thread.h"
#include "server/persistence/persistence_task.h"
#include "game/config.h"

class PersistenceWorker: public Thread {
private:
    Queue<PersistenceTask>& queue;

    Config& config;

public:
    PersistenceWorker(Queue<PersistenceTask>& queue,
                      Config& config);

    void run() override;
};
