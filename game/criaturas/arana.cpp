#include "criaturas/arana.h"

Arana::Arana(const Config& config, int posX, int posY)
    : Criatura("Arana", posX, posY,
               config.getCriaturaVidaMax("arana"),
               config.getCriaturaNivel("arana"),
               config.getCriaturaDanioMin("arana"),
               config.getCriaturaDanioMax("arana"),
               config.getCriaturaFuerza("arana"),
               config.getCriaturaCooldownAtaque(),
               config.getCriaturaCooldownMovimiento()) {}

std::string Arana::getTipo() const { return "arana"; }