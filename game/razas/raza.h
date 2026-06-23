#pragma once
#include <string>

class Config;

class Raza {
    std::string nombre_;
    int constitucionBase_;
    int inteligenciaBase_;
    int fuerzaBase_;
    int agilidadBase_;
    float fRazaVida_;
    float fRazaMana_;
    float fRazaRecuperacion_;

public:
    Raza(const std::string& nombre, const Config& config);

    std::string getNombre() const;
    int getConstitucionBase() const;
    int getInteligenciaBase() const;
    int getFuerzaBase() const;
    int getAgilidadBase() const;
    float getFRazaVida() const;
    float getFRazaMana() const;
    float getFRazaRecuperacion() const;
};
