#include "criaturas/golem.h"

Golem::Golem(const Config& config, int posX, int posY):
    Criatura(
        "Golem", posX, posY, config.getCriaturaVidaMax("golem"),
        config.getCriaturaNivel("golem"), config.getCriaturaDanioMin("golem"),
        config.getCriaturaDanioMax("golem"), config.getCriaturaFuerza("golem"),
        config.getCriaturaCooldownAtaque(),
        config.getCriaturaCooldownMovimiento()) {}

std::string Golem::getTipo() const { return "golem"; }
