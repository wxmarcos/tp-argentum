#include "criaturas/orco.h"

Orco::Orco(int posX, int posY)
    : Criatura("Orco", posX, posY, 100, 1, 5, 10) {}  // TODO: stats desde Config

std::string Orco::getTipo() const { return "Orco"; }
