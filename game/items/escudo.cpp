#include "escudo.h"

Escudo::Escudo(const std::string& nombre, int defensa)
    : nombre(nombre), defensa(defensa) {}

std::string Escudo::getNombre() const { return nombre; }
TipoItem Escudo::getTipo() const { return TipoItem::ESCUDO; }
int Escudo::getDefensa() const { return defensa; }