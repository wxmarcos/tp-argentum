#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

#include "server/persistence/persistence_task.h"

class PersistenceLoader {
public:
    static std::unordered_map<std::string, uint64_t> load_index(
        const std::string& path);

    static std::optional<PersistenceTask> load_player_by_nick(
        const std::string& players_path,
        const std::string& index_path,
        const std::string& nick);
};