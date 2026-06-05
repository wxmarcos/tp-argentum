#pragma once
#include "game/items/item.h"

enum class TipoPocion { VIDA, MANA };

class Jugador;

class Pocion: public Item {
private:
    TipoPocion tipoPocion;
    int cantidad;

public:
    Pocion(TipoPocion tipo, int cantidad);

    std::string getNombre() const override;
    TipoItem getTipo() const override;
    TipoPocion getTipoPocion() const;
    bool esApilable() const override { return true; };
    int getCantidad() const;

    void usar(Jugador& jugador);
};
