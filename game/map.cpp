#include "map.h"
#include <stdexcept>

Mapa::Mapa(int ancho, int alto)
    : ancho(ancho), alto(alto),
    grilla(alto, std::vector<Tile>(ancho)) {}

int Mapa::getAncho() const { return ancho; }
int Mapa::getAlto() const { return alto; }

bool Mapa::esPosicionValida(int x, int y) const {
    return x >= 0 && x < ancho && y >= 0 && y < alto;
}

bool Mapa::esTransitable(int x, int y) const {
    if (!esPosicionValida(x, y)) return false;
    return grilla[y][x].esTransitable;
}

bool Mapa::estaOcupada(int x, int y) const {
    return personajesEnPosicion.count({x, y}) > 0;
}

void Mapa::setTile(int x, int y, TipoTile tipo, bool esTransitable) {
    if (!esPosicionValida(x, y))
        throw std::out_of_range("Posición invalida en setTile");
    grilla[y][x] = Tile(tipo, esTransitable);
}

const Tile& Mapa::getTile(int x, int y) const {
    if (!esPosicionValida(x, y))
        throw std::out_of_range("Posición invalida en getTile");
    return grilla[y][x];
}

void Mapa::agregarPersonaje(Character* personaje) {
    int x = personaje->getPosX();
    int y = personaje->getPosY();
    personajesEnPosicion[{x, y}] = personaje;
}

void Mapa::removerPersonaje(Character* personaje) {
    personajesEnPosicion.erase({personaje->getPosX(), personaje->getPosY()});
}

std::pair<int, int> Mapa::calcularNuevaPosicion(int x, int y, Direccion dir) const {
    switch (dir) {
        case Direccion::NORTE:  return {x,      y - 1};
        case Direccion::SUR:    return {x,      y + 1};
        case Direccion::OESTE:  return {x - 1,  y};
        case Direccion::ESTE:   return {x + 1,  y};
        default: return {x, y};
    }
}

bool Mapa::moverPersonaje(Character* personaje, Direccion dir) {
    personaje->setDireccion(dir);

    int x = personaje->getPosX();
    int y = personaje->getPosY();
    auto [nuevoX, nuevoY] = calcularNuevaPosicion(x, y, dir);

    if (!esTransitable(nuevoX, nuevoY)) return false;
    if (estaOcupada(nuevoX, nuevoY)) return false;

    personajesEnPosicion.erase({x, y});
    personaje->setPosicion(nuevoX, nuevoY);
    personajesEnPosicion[{nuevoX, nuevoY}] = personaje;

    return true;
}

Character* Mapa::getPersonajeEnPosicion(int x, int y) const {
    auto it = personajesEnPosicion.find({x, y});
    if (it == personajesEnPosicion.end()) return nullptr;
    return it->second;
}