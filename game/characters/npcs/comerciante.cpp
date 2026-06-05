#include "characters/npcs/comerciante.h"

Comerciante::Comerciante(int posX, int posY):
    NPC("Comerciante", posX, posY, 1000) {}

void Comerciante::interactuar([[maybe_unused]] Jugador& jugador) {
    // TODO: abrir tienda, comprar/vender items
}
