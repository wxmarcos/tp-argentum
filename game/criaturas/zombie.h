#pragma once
#include "config.h"
#include "criaturas/criatura.h"

class Zombie: public Criatura {
public:
    Zombie(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
