#pragma once

#include <vector>
#include <memory>

#include "game/map.h"
#include "game/characters/character.h"
#include "common/command.h"
#include "common/snapshot.h"

class World {

private:
    Mapa mapa;
    std::vector<std::unique_ptr<Character>> characters;

public:
    World();

    void process_command(const Command& cmd);

    void update();

    Snapshot build_snapshot() const;

    Mapa& get_map();
};