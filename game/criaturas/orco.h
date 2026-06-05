#pragma once
#include "game/config.h"
#include "game/criaturas/criatura.h"

class Orco: public Criatura {
public:
    Orco(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
