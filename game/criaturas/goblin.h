#pragma once
#include "game/config.h"
#include "game/criaturas/criatura.h"

class Goblin: public Criatura {
public:
    Goblin(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
