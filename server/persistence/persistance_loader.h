#pragma once

#include <string>
#include <vector>

#include "server/persistence/persistence_task.h"

class PersistenceLoader {
public:
    static std::vector<PersistenceTask> load_players(const std::string& path);
};
