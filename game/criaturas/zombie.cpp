#include "criaturas/zombie.h"

Zombie::Zombie(int posX, int posY)
    : Criatura("Zombie", posX, posY, 100, 1, 5, 10) {}  // TODO: stats desde Config

std::string Zombie::getTipo() const { return "Zombie"; }
