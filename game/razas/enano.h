#pragma once
#include "razas/raza.h"
#include "config.h"

class Enano : public Raza {
    int constitucionBase;
    int inteligenciaBase;
    int fuerzaBase;
    int agilidadBase;
    float fRazaVida;
    float fRazaMana;
    float fRazaRecuperacion;
public:
    explicit Enano(const Config& config);   
    std::string getNombre() const override;
    int getConstitucionBase() const override;
    int getInteligenciaBase() const override;
    int getFuerzaBase() const override;
    int getAgilidadBase() const override;
    float getFRazaVida() const override;
    float getFRazaMana() const override;
    float getFRazaRecuperacion() const override;
};