#include "criaturas/goblin.h"

Goblin::Goblin(const Config& config, int posX, int posY):
    Criatura(
        "Goblin", posX, posY, config.getCriaturaVidaMax("goblin"),
        config.getCriaturaNivel("goblin"), config.getCriaturaDanioMin("goblin"),
        config.getCriaturaDanioMax("goblin"),
        config.getCriaturaFuerza("goblin"), config.getCriaturaCooldownAtaque(),
        config.getCriaturaCooldownMovimiento()) {}

std::string Goblin::getTipo() const { return "goblin"; }
