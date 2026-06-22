#pragma once
#include "game/characters/criaturas/criatura.h"
#include "game/config.h"

class AranaBlanca: public Criatura {
public:
    AranaBlanca(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
