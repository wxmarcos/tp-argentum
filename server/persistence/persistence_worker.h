#pragma once

#include "common/queue.h"
#include "common/thread.h"
#include "game/config.h"
#include "server/persistence/persistence_job.h"

class PersistenceWorker: public Thread {
private:
    Queue<PersistenceJob>& queue;
    Config& config;

public:
    PersistenceWorker(Queue<PersistenceJob>& queue, Config& config);

    void run() override;
};