#include "characters/npcs/sacerdote.h"

Sacerdote::Sacerdote(int posX, int posY)
    : NPC("Sacerdote", posX, posY, 1000) {}

void Sacerdote::interactuar([[maybe_unused]] Jugador& jugador) {
    // TODO: resucitar al jugador si está muerto
}
