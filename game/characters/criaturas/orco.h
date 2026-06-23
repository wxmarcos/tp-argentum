#pragma once
#include "game/characters/criaturas/criatura.h"
#include "game/config.h"

class Orco: public Criatura {
public:
    Orco(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
