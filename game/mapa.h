#pragma once

#include <map>
#include <optional>
#include <utility>
#include <vector>

#include "characters/character.h"
#include "items/inventario.h"

enum class TipoTile { PISO, AGUA, PARED, PORTAL };

struct Tile {
    TipoTile tipo;
    bool esTransitable;

    Tile(): tipo(TipoTile::PISO), esTransitable(true) {}
    Tile(TipoTile tipo, bool esTransitable):
        tipo(tipo), esTransitable(esTransitable) {}
};

struct DestinoPortal {
    int mapaDestinoId;
    int destinoX, destinoY;
};

class Mapa {
private:
    int ancho;
    int alto;
    bool zonaSegura;
    std::vector<std::vector<Tile>> grilla;
    std::map<std::pair<int, int>, Character*> personajesEnPosicion;
    std::map<std::pair<int, int>, std::vector<SlotInventario>> itemsEnPiso;
    std::map<std::pair<int, int>, DestinoPortal> portales;

    std::pair<int, int> calcularNuevaPosicion(int x, int y,
                                              Direccion dir) const;

public:
    Mapa(int ancho, int alto, bool zonaSegura = false);

    int getAncho() const;
    int getAlto() const;
    bool esZonaSegura() const;
    void setZonaSegura(bool valor);

    bool esPosicionValida(int x, int y) const;
    bool esTransitable(int x, int y) const;
    bool estaOcupada(int x, int y) const;

    void setTile(int x, int y, TipoTile tipo, bool esTransitable);
    const Tile& getTile(int x, int y) const;

    void agregarPersonaje(Character* personaje);
    void removerPersonaje(Character* personaje);
    bool moverPersonaje(Character* personaje, Direccion dir);

    Character* getPersonajeEnPosicion(int x, int y) const;

    void tirarItem(int x, int y, SlotInventario slot);
    std::optional<SlotInventario> tomarItemEnPosicion(int x, int y, int indice);
    const std::vector<SlotInventario>* getItemsEnPosicion(int x, int y) const;
    const std::map<std::pair<int, int>, std::vector<SlotInventario>>&
        getItemsEnPiso() const;

    void registrarPortal(int x, int y, int mapaDestinoId, int destinoX,
                         int destinoY);
    std::optional<DestinoPortal> getPortalDestino(int x, int y) const;
};
