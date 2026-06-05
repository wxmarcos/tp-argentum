#pragma once
#include "characters/npcs/npc.h"

class Comerciante: public NPC {
public:
    Comerciante(int posX, int posY);
    void interactuar(Jugador& jugador) override;  // TODO: compra/venta de items
};
