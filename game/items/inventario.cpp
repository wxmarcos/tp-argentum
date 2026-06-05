#include "game/items/inventario.h"

#include <algorithm>
#include <optional>

#include "game/characters/jugador.h"
#include "game/items/arma.h"
#include "game/items/armadura.h"
#include "game/items/casco.h"
#include "game/items/escudo.h"
#include "game/items/pocion.h"

Inventario::Inventario(int capacidadMax):
    capacidadMax(capacidadMax), slots(capacidadMax),
    armaOBaculoEquipado(nullptr), armaduraEquipada(nullptr),
    cascoEquipado(nullptr), escudoEquipado(nullptr) {}

bool Inventario::estaLleno() const {
    for (const auto& slot : slots) {
        if (!slot.has_value()) {
            return false;
        }
    }

    return true;
}

int Inventario::cantidadSlots() const {
    int cantidad = 0;

    for (const auto& slot : slots) {
        if (slot.has_value()) {
            cantidad++;
        }
    }

    return cantidad;
}

int Inventario::getCapacidadMax() const { return capacidadMax; }

const std::vector<std::optional<SlotInventario>>& Inventario::getSlots() const {
    return slots;
}

int Inventario::buscarSlotApilable(const Item& nuevoItem) const {
    if (!nuevoItem.esApilable()) {
        return -1;
    }

    for (int i = 0; i < static_cast<int>(slots.size()); i++) {
        if (!slots[i].has_value()) {
            continue;
        }

        if (slots[i]->item->getNombre() == nuevoItem.getNombre()) {
            return i;
        }
    }

    return -1;
}

std::optional<int> Inventario::agregar(std::unique_ptr<Item> item,
                                       int cantidad) {
    int slotExistente = buscarSlotApilable(*item);

    if (slotExistente != -1) {
        slots[slotExistente]->cantidad += cantidad;
        return slotExistente;
    }

    for (int i = 0; i < capacidadMax; i++) {
        if (!slots[i].has_value()) {
            slots[i].emplace(std::move(item), cantidad);
            return i;
        }
    }

    return std::nullopt;
}

std::optional<SlotInventario> Inventario::soltar(int indice, int cantidad) {
    if (indice < 0 || indice >= static_cast<int>(slots.size())) {
        return std::nullopt;
    }

    if (!slots[indice].has_value()) {
        return std::nullopt;
    }

    SlotInventario& slot = *slots[indice];

    int cantidadASoltar =
        (cantidad == -1) ? slot.cantidad : std::min(cantidad, slot.cantidad);

    if (cantidadASoltar == slot.cantidad) {
        Item* item = slot.item.get();

        if (item == armaOBaculoEquipado) {
            desequiparArmaOBaculo();
        }

        if (item == armaduraEquipada) {
            desequiparArmadura();
        }

        if (item == cascoEquipado) {
            desequiparCasco();
        }

        if (item == escudoEquipado) {
            desequiparEscudo();
        }

        SlotInventario soltado(std::move(slot.item), cantidadASoltar);

        slots[indice].reset();

        return soltado;
    }

    slot.cantidad -= cantidadASoltar;

    // TODO: necesitamos clone() en Item para poder dividir stacks parciales.
    return std::nullopt;
}

std::vector<SlotInventario> Inventario::soltarTodo() {
    desequiparArmaOBaculo();
    desequiparArmadura();
    desequiparCasco();
    desequiparEscudo();

    std::vector<SlotInventario> todos;

    for (auto& slot : slots) {
        if (!slot.has_value()) {
            continue;
        }

        todos.emplace_back(std::move(slot->item), slot->cantidad);

        slot.reset();
    }

    return todos;
}

bool Inventario::equiparArma(int indice) {
    if (indice < 0 || indice >= static_cast<int>(slots.size())) return false;
    if (!slots[indice].has_value()) return false;
    if (slots[indice]->item->getTipo() != TipoItem::ARMA) return false;

    armaOBaculoEquipado = slots[indice]->item.get();
    return true;
}

bool Inventario::equiparBaculo(int indice) {
    if (indice < 0 || indice >= static_cast<int>(slots.size())) return false;
    if (!slots[indice].has_value()) return false;
    if (slots[indice]->item->getTipo() != TipoItem::BACULO) return false;

    armaOBaculoEquipado = slots[indice]->item.get();
    return true;
}

bool Inventario::equiparArmadura(int indice) {
    if (indice < 0 || indice >= static_cast<int>(slots.size())) return false;
    if (!slots[indice].has_value()) return false;
    if (slots[indice]->item->getTipo() != TipoItem::ARMADURA) return false;

    armaduraEquipada = static_cast<Armadura*>(slots[indice]->item.get());

    return true;
}

bool Inventario::equiparCasco(int indice) {
    if (indice < 0 || indice >= static_cast<int>(slots.size())) return false;
    if (!slots[indice].has_value()) return false;
    if (slots[indice]->item->getTipo() != TipoItem::CASCO) return false;

    cascoEquipado = static_cast<Casco*>(slots[indice]->item.get());

    return true;
}

bool Inventario::equiparEscudo(int indice) {
    if (indice < 0 || indice >= static_cast<int>(slots.size())) return false;
    if (!slots[indice].has_value()) return false;
    if (slots[indice]->item->getTipo() != TipoItem::ESCUDO) return false;

    escudoEquipado = static_cast<Escudo*>(slots[indice]->item.get());

    return true;
}

void Inventario::desequiparArmaOBaculo() { armaOBaculoEquipado = nullptr; }

void Inventario::desequiparArmadura() { armaduraEquipada = nullptr; }

void Inventario::desequiparCasco() { cascoEquipado = nullptr; }

void Inventario::desequiparEscudo() { escudoEquipado = nullptr; }

const Arma* Inventario::getArmaEquipada() const {
    if (!armaOBaculoEquipado) {
        return nullptr;
    }

    if (armaOBaculoEquipado->getTipo() != TipoItem::ARMA) {
        return nullptr;
    }

    return static_cast<const Arma*>(armaOBaculoEquipado);
}

const Baculo* Inventario::getBaculoEquipado() const {
    if (!armaOBaculoEquipado) {
        return nullptr;
    }

    if (armaOBaculoEquipado->getTipo() != TipoItem::BACULO) {
        return nullptr;
    }

    return static_cast<const Baculo*>(armaOBaculoEquipado);
}

const Armadura* Inventario::getArmaduraEquipada() const {
    return armaduraEquipada;
}

const Casco* Inventario::getCascoEquipado() const { return cascoEquipado; }

const Escudo* Inventario::getEscudoEquipado() const { return escudoEquipado; }

std::pair<int, int> Inventario::calcularRangoDefensa() const {
    int min = 0;
    int max = 0;

    if (armaduraEquipada) {
        min += armaduraEquipada->getDefensaMin();
        max += armaduraEquipada->getDefensaMax();
    }

    if (cascoEquipado) {
        min += cascoEquipado->getDefensaMin();
        max += cascoEquipado->getDefensaMax();
    }

    if (escudoEquipado) {
        min += escudoEquipado->getDefensaMin();
        max += escudoEquipado->getDefensaMax();
    }

    return {min, max};
}

std::pair<int, int> Inventario::calcularRangoAtaque() const {
    if (!armaOBaculoEquipado) {
        return {0, 0};
    }

    if (armaOBaculoEquipado->getTipo() == TipoItem::ARMA) {
        const Arma* arma = static_cast<const Arma*>(armaOBaculoEquipado);

        return {arma->getDanioMin(), arma->getDanioMax()};
    }

    if (armaOBaculoEquipado->getTipo() == TipoItem::BACULO) {
        const Baculo* baculo = static_cast<const Baculo*>(armaOBaculoEquipado);

        return {baculo->getEfectoMin(), baculo->getEfectoMax()};
    }

    return {0, 0};
}

bool Inventario::usarPocion(int indice, Jugador& jugador) {
    if (indice < 0 || indice >= static_cast<int>(slots.size())) return false;
    if (!slots[indice].has_value()) return false;
    if (slots[indice]->item->getTipo() != TipoItem::POCION) return false;

    Pocion* pocion = static_cast<Pocion*>(slots[indice]->item.get());

    pocion->usar(jugador);

    slots[indice]->cantidad--;

    if (slots[indice]->cantidad == 0) {
        slots[indice].reset();
    }

    return true;
}

bool Inventario::estaEquipado(const Item* item) const {
    return item == armaOBaculoEquipado || item == armaduraEquipada ||
           item == cascoEquipado || item == escudoEquipado;
}
