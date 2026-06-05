#pragma once

#include <map>
#include <memory>
#include <optional>
#include <string>

#include "game/characters/jugador.h"
#include "game/mapa.h"

struct infoMapasVecinos {
    int norte;
    int sur;
    int este;
    int oeste;
};

class Mundo {
private:
    std::map<int, std::unique_ptr<Mapa>> mapas;
    std::map<int, infoMapasVecinos> mapasVecinos;

    std::optional<int> getMapaVecino(int mapaId, Direccion dir) const;
    std::pair<int, int> posicionEntrada(const Mapa& mapa, Direccion dirOrigen,
                                        int posActual) const;

public:
    void agregarMapa(int id, std::unique_ptr<Mapa> mapa,
                     infoMapasVecinos vecinos);

    Mapa* getMapa(int id);
    const Mapa* getMapa(int id) const;

    void agregarPersonaje(Character* personaje);
    void removerPersonaje(Character* personaje);

    bool moverPersonaje(Character* personaje, Direccion dir);

    void tirarItem(int mapaId, int x, int y, SlotInventario slot);
    std::optional<SlotInventario> tomarItemEnPosicion(int mapaId, int x, int y,
                                                      int indice);
};
