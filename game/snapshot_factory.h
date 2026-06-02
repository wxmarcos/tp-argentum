#pragma once

#include <string>

#include "characters/jugador.h"
#include "common/snapshot.h"

class SnapshotFactory {
public:
    static Snapshot entity_created(const std::string& nombre,
                                   const Jugador* jugador);

    static Snapshot entity_login(const std::string& nombre,
                                 const Jugador* jugador);

    static Snapshot entity_move(const std::string& nombre,
                                const Jugador* jugador);
};