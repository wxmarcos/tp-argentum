#include "armadura.h"

Armadura::Armadura(const std::string& nombre, int defensaMin, int defensaMax)
    : nombre(nombre), defensaMin(defensaMin), defensaMax(defensaMax) {}

std::string Armadura::getNombre() const { return nombre; }
TipoItem Armadura::getTipo() const { return TipoItem::ARMADURA; }
int Armadura::getDefensaMin() const { return defensaMin; }
int Armadura::getDefensaMax() const { return defensaMax; }
bool Armadura::esApilable() const { return false; }