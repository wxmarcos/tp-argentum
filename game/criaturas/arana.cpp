#include "criaturas/arana.h"

Arana::Arana(int posX, int posY)
    : Criatura("Arana", posX, posY, 100, 1, 5, 10) {}  // TODO: stats desde Config

std::string Arana::getTipo() const { return "Arana"; }
