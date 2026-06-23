#pragma once
#include "game/items/item.h"

class Oro: public Item {
private:
    int cantidad;

public:
    explicit Oro(int cantidad);

    std::string getNombre() const override;
    TipoItem getTipo() const override;
    bool esApilable() const override { return true; }
    int getCantidad() const;
};
