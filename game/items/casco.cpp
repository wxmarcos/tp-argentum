#include "game/items/casco.h"

Casco::Casco(const std::string& nombre, int defensaMin, int defensaMax):
    nombre(nombre), defensaMin(defensaMin), defensaMax(defensaMax) {}

std::string Casco::getNombre() const { return nombre; }
TipoItem Casco::getTipo() const { return TipoItem::CASCO; }
int Casco::getDefensaMin() const { return defensaMin; }
int Casco::getDefensaMax() const { return defensaMax; }
bool Casco::esApilable() const { return false; }
