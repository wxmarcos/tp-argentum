#pragma once
#include "charClase.h"

class Guerrero : public charClase {
public:
    std::string getNombre() const override;
    int getMaxVida() const override;
    int getMaxMana() const override;
    bool puedeMeditar() const override;
    bool puedeUsarMagia() const override;
};