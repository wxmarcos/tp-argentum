#pragma once
#include "razas/raza.h"
#include "config.h"

class Gnomo : public Raza {
    int constitucionBase;
    int inteligenciaBase;
    int fuerzaBase;
    int agilidadBase;
    float fRazaVida;
    float fRazaMana;
    float fRazaRecuperacion;
public:
    explicit Gnomo(const Config& config);
    std::string getNombre() const override;
    int getConstitucionBase() const override;
    int getInteligenciaBase() const override;
    int getFuerzaBase() const override;
    int getAgilidadBase() const override;
    float getFRazaVida() const override;
    float getFRazaMana() const override;
    float getFRazaRecuperacion() const override;
};