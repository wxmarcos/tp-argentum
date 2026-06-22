#pragma once
#include "game/characters/criaturas/criatura.h"
#include "game/config.h"

class Zombie: public Criatura {
public:
    Zombie(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
