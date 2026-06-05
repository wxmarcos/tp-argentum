#pragma once
#include "game/config.h"
#include "game/criaturas/criatura.h"

class Esqueleto: public Criatura {
public:
    Esqueleto(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
