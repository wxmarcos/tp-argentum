#include "characters/npcs/banquero.h"

Banquero::Banquero(int posX, int posY)
    : NPC("Banquero", posX, posY, 1000) {}

void Banquero::interactuar([[maybe_unused]] Jugador& jugador) {
    // TODO: acceso a cuenta bancaria del jugador
}
