#pragma once
#include "game/clases/charClase.h"
#include "game/config.h"

class Guerrero: public CharClase {
    float fClaseVida;
    float fClaseMana;
    float fClaseMeditacion;

public:
    explicit Guerrero(const Config& config);
    std::string getNombre() const override;
    float getFClaseVida() const override;
    float getFClaseMana() const override;
    float getFClaseMeditacion() const override;
    bool puedeMeditar() const override;
    bool puedeUsarMagia() const override;
};
