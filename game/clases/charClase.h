#pragma once
#include <string>

class CharClase {
public:
    virtual std::string getNombre() const = 0;
    virtual int getMaxVida() const = 0;
    virtual int getMaxMana() const = 0;
    virtual bool puedeMeditar() const = 0;
    virtual bool puedeUsarMagia() const = 0;
    virtual ~CharClase() = default;
};