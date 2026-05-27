#pragma once
#include "criaturas/criatura.h"

class Arana : public Criatura {
public:
    Arana(int posX, int posY);
    std::string getTipo() const override;
};
