#pragma once
#include <vector>
#include <map>
#include <utility>
#include "characters/character.h"

enum clase TipoTile {
    PISO,
    AGUA,
    PARED,
    PORTAL
};

struct Tile {
    TipoTile tipo;
    bool esTransitable;

    Tile() : tipo(TipoTile::PISO), esTransitable(true) {}
    Tile(TipoTile tipo, bool esTransitable) : tipo(tipo), esTransitable(esTransitable) {}
};

class Mapa {
private:
    int ancho;
    int alto;
    std::vector<std::vector<Tile>> grilla;
    std::map<std::pair<int, int>, Character*> personajesEnPosicion;

    std::pair<int, int> calcularNuevaPosicion(int x, int y, Direccion dir) const;

public:
    Mapa(int ancho, int alto);

    int getAncho() const;
    int getAlto() const;

    bool esPosicionValida(int x, int y) const;
    bool esTransitable(int x, int y) const;
    bool estaOcupada(int x, int y) const;

    void setTile(int x, int y, TipoTile tipo, bool esTransitable);
    const Tile& getTile(int x, int y) const;

    void agregarPersonaje(Character* personaje);
    void removerPersonaje(Character* personaje);
    bool moverPersonaje(Character* personaje, Direccion dir);

    Character* getPersonajeEnPosicion(int x, int y) const;
}