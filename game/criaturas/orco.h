#pragma once
#include "criaturas/criatura.h"

class Orco : public Criatura {
public:
    Orco(int posX, int posY);
    std::string getTipo() const override;
};
