#pragma once

#include <string>
#include <vector>

#include "characters/jugador.h"
#include "common/command/command.h"
#include "server/persistence/persistence_task.h"

class PersistenceTaskFactory {
public:
    static PersistenceTask from_player(const Jugador& jugador);

    static std::vector<std::string> get_affected_players(
        const Command& cmd,
        const std::string& actor);
};