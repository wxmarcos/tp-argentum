#pragma once
#include "item.h"

class Arma : public Item {
private:
    std::string nombre;
    int danioMin;
    int danioMax;
    bool deRango;

public:
    Arma(const std::string& nombre, int danioMin, int danioMax,
        bool deRango = false);

    std::string getNombre() const override;
    TipoItem getTipo() const override;

    int getDanioMin() const;
    int getDanioMax() const;
    bool esDeRango() const;

    int calcularDanio(int fuerza) const;
};