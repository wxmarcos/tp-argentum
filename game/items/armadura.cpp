#include "armadura.h"

Armadura::Armadura(const std::string& nombre, int defensa)
    : nombre(nombre), defensa(defensa) {}

std::string Armadura::getNombre() const { return nombre; }
TipoItem Armadura::getTipo() const { return TipoItem::ARMADURA; }
int Armadura::getDefensa() const { return defensa; }