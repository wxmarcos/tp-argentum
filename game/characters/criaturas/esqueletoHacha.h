#pragma once
#include "game/config.h"
#include "game/characters/criaturas/criatura.h"

class EsqueletoHacha: public Criatura {
public:
    EsqueletoHacha(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
