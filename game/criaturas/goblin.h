#pragma once
#include "criaturas/criatura.h"
#include "config.h"

class Goblin : public Criatura {
public:
    Goblin(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
