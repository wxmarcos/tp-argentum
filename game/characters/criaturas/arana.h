#pragma once
#include "game/characters/criaturas/criatura.h"
#include "game/config.h"

class Arana: public Criatura {
public:
    Arana(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
