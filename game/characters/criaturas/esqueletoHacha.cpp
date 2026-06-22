#include "game/characters/criaturas/esqueletoHacha.h"

EsqueletoHacha::EsqueletoHacha(const Config& config, int posX, int posY):
    Criatura(
        "Esqueleto Hacha", posX, posY, config.getCriaturaVidaMax("esqueleto_hacha"),
        config.getCriaturaNivel("esqueleto_hacha"), config.getCriaturaDanioMin("esqueleto_hacha"),
        config.getCriaturaDanioMax("esqueleto_hacha"),
        config.getCriaturaFuerza("esqueleto_hacha"), config.getCriaturaCooldownAtaque(),
        config.getCriaturaCooldownMovimiento()) {}

std::string EsqueletoHacha::getTipo() const { return "esqueleto_hacha"; }
