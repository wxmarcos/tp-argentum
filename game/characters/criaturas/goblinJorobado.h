#pragma once
#include "game/characters/criaturas/criatura.h"
#include "game/config.h"

class GoblinJorobado: public Criatura {
public:
    GoblinJorobado(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
