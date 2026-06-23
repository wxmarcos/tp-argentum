#pragma once
#include "game/characters/criaturas/criatura.h"
#include "game/config.h"

class EsqueletoHacha: public Criatura {
public:
    EsqueletoHacha(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
