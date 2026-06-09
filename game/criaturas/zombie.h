#pragma once
#include "game/config.h"
#include "game/criaturas/criatura.h"

class Zombie: public Criatura {
public:
    Zombie(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
