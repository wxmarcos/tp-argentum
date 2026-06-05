#pragma once

#include <string>

#include "common/queue.h"
#include "common/thread.h"
#include "server/persistence/persistence_task.h"

class PersistenceWorker: public Thread {
private:
    Queue<PersistenceTask>& queue;

    std::string save_file_path;

public:
    PersistenceWorker(Queue<PersistenceTask>& queue,
                      const std::string& save_file_path);

    void run() override;
};
