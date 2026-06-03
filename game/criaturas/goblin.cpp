#include "criaturas/goblin.h"

Goblin::Goblin(int posX, int posY)
    : Criatura("Goblin", posX, posY, 100, 1, 5, 10) {}  // TODO: stats desde Config

std::string Goblin::getTipo() const { return "Goblin"; }
