#pragma once
#include "item.h"

class Armadura : public Item {
private:
    std::string nombre;
    int defensaMin;
    int defensaMax;

public:
    Armadura(const std::string& nombre, int defensaMin, int defensaMax);

    std::string getNombre() const override;
    TipoItem getTipo() const override;
    int getDefensaMin() const;
    int getDefensaMax() const;
    bool esApilable() const override;
};