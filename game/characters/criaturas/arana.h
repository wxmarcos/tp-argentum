#pragma once
#include "game/config.h"
#include "game/characters/criaturas/criatura.h"

class Arana: public Criatura {
public:
    Arana(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
