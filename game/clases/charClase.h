#pragma once
#include <string>

class charClase {
public:
    virtual std::string getNombre() const = 0;
    virtual float getFClaseVida() const = 0;
    virtual float getFClaseMana() const = 0;
    virtual float getFClaseMeditacion() const = 0;
    virtual bool puedeMeditar() const = 0;
    virtual bool puedeUsarMagia() const = 0;
    virtual ~charClase() = default;
};
