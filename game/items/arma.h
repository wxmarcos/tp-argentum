#pragma once
#include "item.h"

class Arma : public Item {
private:
    std::string nombre;
    int danioMin;
    int danioMax;

public:
    Arma(const std::string& nombre, int danioMin, int danioMax);

    std::string getNombre() const override;
    TipoItem getTipo() const override;

    int getDanioMin() const;
    int getDanioMax() const;
    int calcularDanio(int fuerza) const;
};