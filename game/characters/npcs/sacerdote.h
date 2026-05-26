#pragma once
#include "characters/npcs/npc.h"

class Sacerdote : public NPC {
public:
    Sacerdote(int posX, int posY);
    void interactuar(Jugador& jugador) override;  // TODO: resucitar jugador
};
