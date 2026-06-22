#pragma once
#include "game/characters/criaturas/criatura.h"
#include "game/config.h"

class GolemDemoniaco: public Criatura {
public:
    GolemDemoniaco(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
