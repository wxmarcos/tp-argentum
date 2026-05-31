#include "escudo.h"

Escudo::Escudo(const std::string& nombre, int defensaMin, int defensaMax)
    : nombre(nombre), defensaMin(defensaMin), defensaMax(defensaMax) {}

std::string Escudo::getNombre() const { return nombre; }
TipoItem Escudo::getTipo() const { return TipoItem::ESCUDO; }
int Escudo::getDefensaMin() const { return defensaMin; }
int Escudo::getDefensaMax() const { return defensaMax; }
bool Escudo::esApilable() const { return false; }