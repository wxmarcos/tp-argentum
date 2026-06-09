#pragma once
#include <string>
#include <vector>

#include "items/inventario.h"

class CuentaBanco {
private:
    std::string nombreJugador;
    int oro;
    std::vector<SlotInventario> items;

public:
    explicit CuentaBanco(const std::string& nombreJugador);
    bool depositarOro(int cantidad);
    bool retirarOro(int cantidad);
    int getOro() const;
    bool depositarItem(SlotInventario slot);
    std::optional<SlotInventario> retirarItem(int indice);
    const std::vector<SlotInventario>& getItems() const;
};
