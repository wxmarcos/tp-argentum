#include "game/characters/criaturas/aranaBlanca.h"

AranaBlanca::AranaBlanca(const Config& config, int posX, int posY):
    Criatura("Arana Blanca", posX, posY,
             config.getCriaturaVidaMax("arana_blanca"),
             config.getCriaturaNivel("arana_blanca"),
             config.getCriaturaDanioMin("arana_blanca"),
             config.getCriaturaDanioMax("arana_blanca"),
             config.getCriaturaFuerza("arana_blanca"),
             config.getCriaturaCooldownAtaque(),
             config.getCriaturaCooldownMovimiento()) {}

std::string AranaBlanca::getTipo() const { return "arana_blanca"; }
