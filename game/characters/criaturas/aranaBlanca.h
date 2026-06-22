#pragma once
#include "game/config.h"
#include "game/characters/criaturas/criatura.h"

class AranaBlanca: public Criatura {
public:
    AranaBlanca(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
