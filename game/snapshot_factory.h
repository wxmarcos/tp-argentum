#pragma once

#include "characters/jugador.h"
#include "common/snapshot/snapshot.h"
#include "items/inventario.h"
class Jugador;

class SnapshotFactory {
public:
    static Snapshot player_stats_from_player(const Jugador& jugador);
    static Snapshot player_inventory_from_player(const Jugador& jugador);
    static Snapshot player_inventory_slot_from_player(const Jugador& jugador,
                                                      int slot_id);
};
