#pragma once

#include "common/snapshot/snapshot.h"

class Jugador;

class SnapshotFactory {
public:
    static Snapshot player_stats_from_player(const Jugador& jugador);
};