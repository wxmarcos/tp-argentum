#pragma once
#include "criaturas/criatura.h"

class Esqueleto : public Criatura {
public:
    Esqueleto(int posX, int posY);
    std::string getTipo() const override;
};
