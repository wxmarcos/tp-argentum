#include "game/characters/criaturas/goblinJorobado.h"

GoblinJorobado::GoblinJorobado(const Config& config, int posX, int posY):
    Criatura("Goblin Jorobado", posX, posY,
             config.getCriaturaVidaMax("goblin_jorobado"),
             config.getCriaturaNivel("goblin_jorobado"),
             config.getCriaturaDanioMin("goblin_jorobado"),
             config.getCriaturaDanioMax("goblin_jorobado"),
             config.getCriaturaFuerza("goblin_jorobado"),
             config.getCriaturaCooldownAtaque(),
             config.getCriaturaCooldownMovimiento()) {}

std::string GoblinJorobado::getTipo() const { return "goblin_jorobado"; }
