#pragma once
#include "characters/npcs/npc.h"

class Banquero : public NPC {
public:
    Banquero(int posX, int posY);
    void interactuar(Jugador& jugador) override;  // TODO: depositar/retirar oro e items
};
