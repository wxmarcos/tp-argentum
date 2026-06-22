#pragma once
#include "game/config.h"
#include "game/characters/criaturas/criatura.h"

class GolemDemoniaco: public Criatura {
public:
    GolemDemoniaco(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
