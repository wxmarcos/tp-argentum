#include "criaturas/zombie.h"

Zombie::Zombie(const Config& config, int posX, int posY)
    : Criatura("Zombie", posX, posY,
               config.getCriaturaVidaMax("zombie"),
               config.getCriaturaNivel("zombie"),
               config.getCriaturaDanioMin("zombie"),
               config.getCriaturaDanioMax("zombie"),
               config.getCriaturaFuerza("zombie"),
               config.getCriaturaCooldownAtaque(),
               config.getCriaturaCooldownMovimiento()) {}

std::string Zombie::getTipo() const { return "zombie"; }