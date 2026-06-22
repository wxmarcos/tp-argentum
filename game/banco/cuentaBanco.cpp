#include "banco/cuentaBanco.h"

#include <algorithm>

CuentaBanco::CuentaBanco(const std::string& nombreJugador):
    nombreJugador(nombreJugador), oro(0) {}

bool CuentaBanco::depositarOro(int cantidad) {
    if (cantidad <= 0) return false;
    oro += cantidad;
    return true;
}

bool CuentaBanco::retirarOro(int cantidad) {
    if (cantidad <= 0 || cantidad > oro) return false;
    oro -= cantidad;
    return true;
}

int CuentaBanco::getOro() const { return oro; }

bool CuentaBanco::depositarItem(SlotInventario slot) {
    items.push_back(std::move(slot));
    return true;
}

std::optional<SlotInventario> CuentaBanco::retirarItem(int indice) {
    if (indice < 0 || indice >= static_cast<int>(items.size()))
        return std::nullopt;
    SlotInventario resultado = std::move(items[indice]);
    items.erase(items.begin() + indice);
    return resultado;
}
void CuentaBanco::setOro(int cantidad) { oro = std::max(0, cantidad); }

void CuentaBanco::limpiarItems() { items.clear(); }

const std::vector<SlotInventario>& CuentaBanco::getItems() const {
    return items;
}
