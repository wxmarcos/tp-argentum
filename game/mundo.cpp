#include "game/mundo.h"

void Mundo::agregarMapa(int id, std::unique_ptr<Mapa> mapa,
                        infoMapasVecinos infoVecinos) {
    mapas[id] = std::move(mapa);
    mapasVecinos[id] = infoVecinos;
}

Mapa* Mundo::getMapa(int id) {
    auto it = mapas.find(id);
    return (it != mapas.end()) ? it->second.get() : nullptr;
}

const Mapa* Mundo::getMapa(int id) const {
    auto it = mapas.find(id);
    return (it != mapas.end()) ? it->second.get() : nullptr;
}

std::optional<int> Mundo::getMapaVecino(int mapaId, Direccion dir) const {
    auto it = mapasVecinos.find(mapaId);
    if (it == mapasVecinos.end()) return std::nullopt;

    const infoMapasVecinos& vecinos = it->second;
    int vecinoId = -1;
    switch (dir) {
        case Direccion::NORTE:
            vecinoId = vecinos.norte;
            break;
        case Direccion::SUR:
            vecinoId = vecinos.sur;
            break;
        case Direccion::ESTE:
            vecinoId = vecinos.este;
            break;
        case Direccion::OESTE:
            vecinoId = vecinos.oeste;
            break;
        default:
            return std::nullopt;
    }
    if (vecinoId == -1) return std::nullopt;
    return vecinoId;
}

std::pair<int, int> Mundo::posicionEntrada(const Mapa& mapa,
                                           Direccion dirOrigen,
                                           int posActual) const {
    int ancho = mapa.getAncho();
    int alto = mapa.getAlto();

    switch (dirOrigen) {
        case Direccion::NORTE:
            return {posActual, alto - 1};
        case Direccion::SUR:
            return {posActual, 0};
        case Direccion::ESTE:
            return {0, posActual};
        case Direccion::OESTE:
            return {ancho - 1, posActual};
    }
    return {0, 0};
}

void Mundo::agregarPersonaje(Character* personaje) {
    Mapa* mapa = getMapa(personaje->getMapaId());
    if (mapa) {
        mapa->agregarPersonaje(personaje);
    }
}

void Mundo::removerPersonaje(Character* personaje) {
    Mapa* mapa = getMapa(personaje->getMapaId());
    if (mapa) {
        mapa->removerPersonaje(personaje);
    }
}

bool Mundo::moverPersonaje(Character* personaje, Direccion dir) {
    int mapaActualId = personaje->getMapaId();
    Mapa* mapaActual = getMapa(mapaActualId);
    if (!mapaActual) return false;

    personaje->setDireccion(dir);

    int x = personaje->getPosX();
    int y = personaje->getPosY();
    int ancho = mapaActual->getAncho();
    int alto = mapaActual->getAlto();

    bool saleBorde = false;
    switch (dir) {
        case Direccion::NORTE:
            saleBorde = (y == 0);
            break;
        case Direccion::SUR:
            saleBorde = (y == alto - 1);
            break;
        case Direccion::ESTE:
            saleBorde = (x == ancho - 1);
            break;
        case Direccion::OESTE:
            saleBorde = (x == 0);
            break;
    }

    if (!saleBorde) {
        bool movido = mapaActual->moverPersonaje(personaje, dir);
        if (!movido) return false;

        // Verificar si pisó un portal
        auto destino = mapaActual->getPortalDestino(personaje->getPosX(),
                                                    personaje->getPosY());
        if (destino) {
            Mapa* mapaDestino = getMapa(destino->mapaDestinoId);
            if (mapaDestino &&
                mapaDestino->esTransitable(destino->destinoX,
                                           destino->destinoY) &&
                !mapaDestino->estaOcupada(destino->destinoX,
                                          destino->destinoY)) {
                mapaActual->removerPersonaje(personaje);
                personaje->setPosicion(destino->destinoX, destino->destinoY);
                personaje->setMapaId(destino->mapaDestinoId);
                mapaDestino->agregarPersonaje(personaje);
            }
        }

        return true;
    }

    // Movimiento entre mapas por borde
    auto vecinoId = getMapaVecino(mapaActualId, dir);
    if (!vecinoId) return false;

    Mapa* mapaVecino = getMapa(*vecinoId);
    if (!mapaVecino) return false;

    int posActual = (dir == Direccion::NORTE || dir == Direccion::SUR) ? x : y;
    auto [entradaX, entradaY] = posicionEntrada(*mapaVecino, dir, posActual);

    if (!mapaVecino->esTransitable(entradaX, entradaY) ||
        mapaVecino->estaOcupada(entradaX, entradaY)) {
        return false;
    }

    mapaActual->removerPersonaje(personaje);
    personaje->setPosicion(entradaX, entradaY);
    personaje->setMapaId(*vecinoId);
    mapaVecino->agregarPersonaje(personaje);

    return true;
}

void Mundo::tirarItem(int mapaId, int x, int y, SlotInventario slot) {
    Mapa* mapa = getMapa(mapaId);
    if (mapa) {
        mapa->tirarItem(x, y, std::move(slot));
    }
}

std::optional<SlotInventario> Mundo::tomarItemEnPosicion(int mapaId, int x,
                                                         int y, int indice) {
    Mapa* mapa = getMapa(mapaId);
    if (!mapa) {
        return std::nullopt;
    }
    return mapa->tomarItemEnPosicion(x, y, indice);
}
