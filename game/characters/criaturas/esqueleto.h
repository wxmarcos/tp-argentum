#pragma once
#include "game/characters/criaturas/criatura.h"
#include "game/config.h"

class Esqueleto: public Criatura {
public:
    Esqueleto(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
