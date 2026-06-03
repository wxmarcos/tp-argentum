#pragma once
#include "criaturas/criatura.h"
#include "config.h"

class Zombie : public Criatura {
public:
    Zombie(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
