#include "casco.h"

Casco::Casco(const std::string& nombre, int defensa)
    : nombre(nombre), defensa(defensa) {}

std::string Casco::getNombre() const { return nombre; }
TipoItem Casco::getTipo() const { return TipoItem::CASCO; }
int Casco::getDefensa() const { return defensa; }