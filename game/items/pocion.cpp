#include "pocion.h"
#include "characters/jugador.h"

Pocion::Pocion(TipoPocion tipo, int cantidad)
    : tipoPocion(tipo), cantidad(cantidad) {}

std::string Pocion::getNombre() const {
    return (tipoPocion == TipoPocion::VIDA) ? "Poción de Vida" : "Poción de Maná";
}

TipoItem Pocion::getTipo() const { return TipoItem::POCION; }
TipoPocion Pocion::getTipoPocion() const { return tipoPocion; }
int Pocion::getCantidad() const { return cantidad; }

void Pocion::usar(Jugador& jugador) {
    if (!jugador.estaVivo()) return;
    if (tipoPocion == TipoPocion::VIDA) {
        jugador.curar(cantidad);
    } else {
        jugador.recuperarMana(cantidad);
    }
}