#pragma once
#include "criaturas/criatura.h"
#include "config.h"

class Esqueleto : public Criatura {
public:
    Esqueleto(const Config& config, int posX, int posY);
    std::string getTipo() const override;
};
