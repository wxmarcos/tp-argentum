#include "character.h"
#include <algorithm>

Character::Character(const std::string& nombre, int posX, int posY, int vidaMax)
    : nombre(nombre), mapaId(0), posX(posX), posY(posY),
    direccion(Direccion::SUR), vidaActual(vidaMax), vidaMax(vidaMax), vivo(true) {}

const std::string& Character::getNombre() const { return nombre; }
int Character::getMapaId() const { return mapaId; }
int Character::getPosX() const { return posX; }
int Character::getPosY() const { return posY; }
Direccion Character::getDireccion() const { return direccion; }
int Character::getVidaActual() const { return vidaActual; }
int Character::getVidaMax() const { return vidaMax; }
bool Character::estaVivo() const { return vivo; }

void Character::setMapaId(int id) { mapaId = id; }

void Character::setPosicion(int x, int y) {
    posX = x;
    posY = y;
}

void Character::setDireccion(Direccion dir) {
    direccion = dir;
}

void Character::recibirDanio(int danio) {
    if (!vivo) return;

    vidaActual = std::max(0, vidaActual - danio);
    if (vidaActual == 0) morir();
}

void Character::curar(int cantidad) {
    if (!vivo) return;
    vidaActual = std::min(vidaMax, vidaActual + cantidad);
}

void Character::morir() {
    vivo = false;
    vidaActual = 0;
}

void Character::revivir(int vidaInicial) {
    vivo = true;
    vidaActual = std::min(vidaInicial, vidaMax);
}