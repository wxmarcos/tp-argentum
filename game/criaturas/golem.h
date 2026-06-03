#pragma once
#include "criaturas/criatura.h"
#include "config.h"

class Golem : public Criatura {
public:
    Golem(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
