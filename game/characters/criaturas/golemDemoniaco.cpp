#include "game/characters/criaturas/golemDemoniaco.h"

GolemDemoniaco::GolemDemoniaco(const Config& config, int posX, int posY):
    Criatura(
        "Golem Demoniaco", posX, posY, config.getCriaturaVidaMax("golem_demoniaco"),
        config.getCriaturaNivel("golem_demoniaco"), config.getCriaturaDanioMin("golem_demoniaco"),
        config.getCriaturaDanioMax("golem_demoniaco"),
        config.getCriaturaFuerza("golem_demoniaco"), config.getCriaturaCooldownAtaque(),
        config.getCriaturaCooldownMovimiento()) {}

std::string GolemDemoniaco::getTipo() const { return "golem_demoniaco"; }
