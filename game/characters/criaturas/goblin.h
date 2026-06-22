#pragma once
#include "game/characters/criaturas/criatura.h"
#include "game/config.h"

class Goblin: public Criatura {
public:
    Goblin(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
