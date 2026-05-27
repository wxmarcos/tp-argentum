#include "characters/npcs/npc.h"

NPC::NPC(const std::string& nombre, int posX, int posY, int vida)
    : Character(nombre, posX, posY, vida) {}
