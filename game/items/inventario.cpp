#include "inventario.h"
#include "arma.h"
#include "armadura.h"
#include "casco.h"
#include "escudo.h"
#include "pocion.h"
#include "characters/jugador.h"

#include <optional>

Inventario::Inventario()
    : armaEquipada(nullptr), armaduraEquipada(nullptr),
      cascoEquipado(nullptr), escudoEquipado(nullptr) {}

bool Inventario::estaLleno() const {
    return (int)slots.size() >= CAPACIDAD_MAX;
}

int Inventario::cantidadSlots() const { return (int)slots.size(); }

const std::vector<SlotInventario>& Inventario::getSlots() const { return slots; }

int Inventario::buscarSlotApilable(const Item& nuevoItem) const {
    if (!nuevoItem.esApilable()) return -1;
    for (int i = 0; i < (int)slots.size(); i++) {
        if (slots[i].item->getNombre() == nuevoItem.getNombre()) return i;
    }
    return -1;
}

bool Inventario::agregar(std::unique_ptr<Item> item, int cantidad) {
    int slotExistente = buscarSlotApilable(*item);
    if (slotExistente != -1) {
        slots[slotExistente].cantidad += cantidad;
        return true;
    }
    if (estaLleno()) return false;
    slots.emplace_back(std::move(item), cantidad);
    return true;
}

std::optional<SlotInventario> Inventario::soltar(int indice, int cantidad) {
    if (indice < 0 || indice >= (int)slots.size()) return std::nullopt;

    SlotInventario& slot = slots[indice];
    int cantidadASoltar = (cantidad == -1) ? slot.cantidad : std::min(cantidad, slot.cantidad);

    if (cantidadASoltar == slot.cantidad) {
        Item* item = slot.item.get();
        if (item == armaEquipada) desequiparArma();
        if (item == armaduraEquipada) desequiparArmadura();
        if (item == cascoEquipado) desequiparCasco();
        if (item == escudoEquipado) desequiparEscudo();

        SlotInventario soltado(std::move(slot.item), cantidadASoltar);
        slots.erase(slots.begin() + indice);
        return soltado;
    } else {
        slot.cantidad -= cantidadASoltar;
        // TODO: necesitamos clone() en Item para poder dividir stacks parciales
        return std::nullopt;
    }
}

std::vector<SlotInventario> Inventario::soltarTodo() {
    desequiparArma();
    desequiparArmadura();
    desequiparCasco();
    desequiparEscudo();

    std::vector<SlotInventario> todos;
    for (auto& slot : slots) {
        todos.emplace_back(std::move(slot.item), slot.cantidad);
    }
    slots.clear();
    return todos;
}

bool Inventario::equiparArma(int indice) {
    if (indice < 0 || indice >= (int)slots.size()) return false;
    if (slots[indice].item->getTipo() != TipoItem::ARMA) return false;
    armaEquipada = static_cast<Arma*>(slots[indice].item.get());
    return true;
}

bool Inventario::equiparArmadura(int indice) {
    if (indice < 0 || indice >= (int)slots.size()) return false;
    if (slots[indice].item->getTipo() != TipoItem::ARMADURA) return false;
    armaduraEquipada = static_cast<Armadura*>(slots[indice].item.get());
    return true;
}

bool Inventario::equiparCasco(int indice) {
    if (indice < 0 || indice >= (int)slots.size()) return false;
    if (slots[indice].item->getTipo() != TipoItem::CASCO) return false;
    cascoEquipado = static_cast<Casco*>(slots[indice].item.get());
    return true;
}

bool Inventario::equiparEscudo(int indice) {
    if (indice < 0 || indice >= (int)slots.size()) return false;
    if (slots[indice].item->getTipo() != TipoItem::ESCUDO) return false;
    escudoEquipado = static_cast<Escudo*>(slots[indice].item.get());
    return true;
}

void Inventario::desequiparArma() { armaEquipada = nullptr; }
void Inventario::desequiparArmadura() { armaduraEquipada = nullptr; }
void Inventario::desequiparCasco() { cascoEquipado = nullptr; }
void Inventario::desequiparEscudo() { escudoEquipado = nullptr; }

const Arma* Inventario::getArmaEquipada() const { return armaEquipada; }
const Armadura* Inventario::getArmaduraEquipada() const { return armaduraEquipada; }
const Casco* Inventario::getCascoEquipado() const { return cascoEquipado; }
const Escudo* Inventario::getEscudoEquipado() const { return escudoEquipado; }

int Inventario::calcularDefensaTotal() const {
    int total = 0;
    if (armaduraEquipada) total += armaduraEquipada->getDefensa();
    if (cascoEquipado) total += cascoEquipado->getDefensa();
    if (escudoEquipado) total += escudoEquipado->getDefensa();
    return total;
}

bool Inventario::usarPocion(int indice, Jugador& jugador) {
    if (indice < 0 || indice >= (int)slots.size()) return false;
    if (slots[indice].item->getTipo() != TipoItem::POCION) return false;

    Pocion* pocion = static_cast<Pocion*>(slots[indice].item.get());
    pocion->usar(jugador);
    
    slots[indice].cantidad--;
    if (slots[indice].cantidad == 0) slots.erase(slots.begin() + indice);
    return true;
}