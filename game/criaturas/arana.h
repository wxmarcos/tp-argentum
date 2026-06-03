#pragma once
#include "criaturas/criatura.h"
#include "config.h"

class Arana : public Criatura {
public:
    Arana(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
