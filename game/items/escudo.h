#pragma once
#include "item.h"

class Escudo : public Item {
private:
    std::string nombre;
    int defensa;

public:
    Escudo(const std::string& nombre, int defensa);

    std::string getNombre() const override;
    TipoItem getTipo() const override;
    int getDefensa() const;
};