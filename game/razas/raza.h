#pragma once
#include <string>

class Raza {
public:
    virtual std::string getNombre() const = 0;
    virtual int getMaxVida() const = 0;
    virtual int getMaxMana() const = 0;
    virtual float getFRecuperacion() const = 0;
    virtual ~Raza() = default;
};