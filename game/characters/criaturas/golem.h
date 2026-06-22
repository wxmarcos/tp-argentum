#pragma once
#include "game/characters/criaturas/criatura.h"
#include "game/config.h"

class Golem: public Criatura {
public:
    Golem(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
