#pragma once
#include "criaturas/criatura.h"

class Goblin : public Criatura {
public:
    Goblin(int posX, int posY);
    std::string getTipo() const override;
};
