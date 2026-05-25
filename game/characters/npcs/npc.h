#pragma once
#include "characters/character.h"

class Jugador;

class NPC : public Character {
public:
    NPC(const std::string& nombre, int posX, int posY, int vida);
    virtual ~NPC() = default;
    virtual void interactuar(Jugador& jugador) = 0;
};
