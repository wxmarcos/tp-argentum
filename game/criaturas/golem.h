#pragma once
#include "criaturas/criatura.h"

class Golem : public Criatura {
public:
    Golem(int posX, int posY);
    std::string getTipo() const override;
};
