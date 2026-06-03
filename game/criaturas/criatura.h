#pragma once
#include "characters/character.h"
#include <string>

class Criatura : public Character {
protected:
    int nivel;
    int danioMin;
    int danioMax;
    int fuerza;

public:
    Criatura(const std::string& nombre, int posX, int posY,
             int vidaMax, int nivel, int danioMin, int danioMax, int fuerza);
    virtual ~Criatura() = default;
    virtual std::string getTipo() const = 0;
    
    int getNivel() const;
    int getDanioMin() const;
    int getDanioMax() const;
};
