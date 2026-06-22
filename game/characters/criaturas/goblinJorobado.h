#pragma once
#include "game/config.h"
#include "game/characters/criaturas/criatura.h"

class GoblinJorobado: public Criatura {
public:
    GoblinJorobado(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
