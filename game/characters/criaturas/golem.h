#pragma once
#include "game/config.h"
#include "game/characters/criaturas/criatura.h"

class Golem: public Criatura {
public:
    Golem(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
