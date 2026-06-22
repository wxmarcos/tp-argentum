#pragma once

#include <map>
#include <string>

#include "game/clan.h"
#include "server/persistence/persistence_task.h"

enum class PersistenceJobType { PLAYER, CLANS };

struct PersistenceJob {
    PersistenceJobType type;
    PersistenceTask player;
    std::map<std::string, Clan> clanes;

    static PersistenceJob player_job(const PersistenceTask& task) {
        PersistenceJob job{};
        job.type = PersistenceJobType::PLAYER;
        job.player = task;
        return job;
    }

    static PersistenceJob clans_job(const std::map<std::string, Clan>& clanes) {
        PersistenceJob job{};
        job.type = PersistenceJobType::CLANS;
        job.clanes = clanes;
        return job;
    }
};
