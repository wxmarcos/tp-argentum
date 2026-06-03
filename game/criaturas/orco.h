#pragma once
#include "criaturas/criatura.h"
#include "config.h"

class Orco : public Criatura {
public:
    Orco(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
