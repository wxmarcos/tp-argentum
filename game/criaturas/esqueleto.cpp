#include "criaturas/esqueleto.h"

Esqueleto::Esqueleto(const Config& config, int posX, int posY):
    Criatura("Esqueleto", posX, posY, config.getCriaturaVidaMax("esqueleto"),
             config.getCriaturaNivel("esqueleto"),
             config.getCriaturaDanioMin("esqueleto"),
             config.getCriaturaDanioMax("esqueleto"),
             config.getCriaturaFuerza("esqueleto"),
             config.getCriaturaCooldownAtaque(),
             config.getCriaturaCooldownMovimiento()) {}

std::string Esqueleto::getTipo() const { return "esqueleto"; }
