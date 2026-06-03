#include "criaturas/esqueleto.h"

Esqueleto::Esqueleto(int posX, int posY)
    : Criatura("Esqueleto", posX, posY, 100, 1, 5, 10) {}  // TODO: stats desde Config

std::string Esqueleto::getTipo() const { return "Esqueleto"; }
