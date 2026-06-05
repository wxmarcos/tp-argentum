#pragma once
#include <memory>
#include <optional>
#include <vector>

#include "game/items/baculo.h"
#include "game/items/item.h"

class Arma;
class Armadura;
class Casco;
class Escudo;
class Jugador;

struct SlotInventario {
    std::unique_ptr<Item> item;
    int cantidad;

    explicit SlotInventario(std::unique_ptr<Item> item, int cantidad = 1):
        item(std::move(item)), cantidad(cantidad) {}
};

class Inventario {
private:
    int capacidadMax;

    std::vector<std::optional<SlotInventario>> slots;
    Item* armaOBaculoEquipado;
    Armadura* armaduraEquipada;
    Casco* cascoEquipado;
    Escudo* escudoEquipado;

    int buscarSlotApilable(const Item& item) const;

public:
    explicit Inventario(int capacidadMax = 30);

    bool estaLleno() const;
    int cantidadSlots() const;
    int getCapacidadMax() const;
    const std::vector<std::optional<SlotInventario>>& getSlots() const;

    std::optional<int> agregar(std::unique_ptr<Item> item, int cantidad = 1);
    std::optional<SlotInventario> soltar(int indice, int cantidad = -1);
    std::vector<SlotInventario> soltarTodo();

    bool equiparArma(int indice);
    bool equiparBaculo(int indice);
    bool equiparArmadura(int indice);
    bool equiparCasco(int indice);
    bool equiparEscudo(int indice);

    void desequiparArmaOBaculo();
    void desequiparArmadura();
    void desequiparCasco();
    void desequiparEscudo();

    const Arma* getArmaEquipada() const;
    const Baculo* getBaculoEquipado() const;
    const Armadura* getArmaduraEquipada() const;
    const Casco* getCascoEquipado() const;
    const Escudo* getEscudoEquipado() const;

    std::pair<int, int> calcularRangoDefensa() const;
    std::pair<int, int> calcularRangoAtaque() const;

    bool usarPocion(int indice, Jugador& jugador);
    bool estaEquipado(const Item* item) const;
};
