#pragma once
#include "criaturas/criatura.h"

class Zombie : public Criatura {
public:
    Zombie(int posX, int posY);
    std::string getTipo() const override;
};
