#include "criaturas/golem.h"

Golem::Golem(int posX, int posY)
    : Criatura("Golem", posX, posY, 100, 1, 5, 10) {}  // TODO: stats desde Config

std::string Golem::getTipo() const { return "Golem"; }
