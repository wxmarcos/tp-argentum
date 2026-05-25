#pragma once
#include "raza.h"

class Enano : public Raza {
public:
    std::string getNombre() const override;
    int getMaxVida() const override;
    int getMaxMana() const override;
    float getFRecuperacion() const override;
};