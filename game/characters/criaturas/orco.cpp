#include "game/characters/criaturas/orco.h"

Orco::Orco(const Config& config, int posX, int posY):
    Criatura(
        "Orco", posX, posY, config.getCriaturaVidaMax("orco"),
        config.getCriaturaNivel("orco"), config.getCriaturaDanioMin("orco"),
        config.getCriaturaDanioMax("orco"), config.getCriaturaFuerza("orco"),
        config.getCriaturaCooldownAtaque(),
        config.getCriaturaCooldownMovimiento()) {}

std::string Orco::getTipo() const { return "orco"; }
