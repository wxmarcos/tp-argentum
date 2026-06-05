#pragma once
#include <string>

class Raza {
public:
    virtual std::string getNombre() const = 0;
    virtual int getConstitucionBase() const = 0;
    virtual int getInteligenciaBase() const = 0;
    virtual int getFuerzaBase() const = 0;
    virtual int getAgilidadBase() const = 0;
    virtual float getFRazaVida() const = 0;
    virtual float getFRazaMana() const = 0;
    virtual float getFRazaRecuperacion() const = 0;
    virtual ~Raza() = default;
};
