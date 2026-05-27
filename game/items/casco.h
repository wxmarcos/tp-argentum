#pragma once
#include "item.h"

class Casco : public Item {
private:
    std::string nombre;
    int defensa;

public:
    Casco(const std::string& nombre, int defensa);

    std::string getNombre() const override;
    TipoItem getTipo() const override;
    int getDefensa() const;
};