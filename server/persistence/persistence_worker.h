#pragma once

#include <string>

#include "common/thread.h"
#include "common/queue.h"

#include "persistence_task.h"

class PersistenceWorker : public Thread {

private:
    Queue<PersistenceTask>& queue;

    std::string save_file_path;

public:
    PersistenceWorker(
        Queue<PersistenceTask>& queue,
        const std::string& save_file_path);

    void run() override;
};