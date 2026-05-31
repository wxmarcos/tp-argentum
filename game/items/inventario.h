#pragma once
#include <vector>
#include <memory>
#include <optional>
#include "item.h"
#include "baculo.h"

class Arma;
class Armadura;
class Casco;
class Escudo;
class Jugador;

struct SlotInventario {
    std::unique_ptr<Item> item;
    int cantidad;

    SlotInventario(std::unique_ptr<Item> item, int cantidad = 1)
        : item(std::move(item)), cantidad(cantidad) {}
};

class Inventario {
private:
    static constexpr int CAPACIDAD_MAX = 30;        // TODO: cargar desde Config

    std::vector<SlotInventario> slots;
    Item* armaOBaculoEquipado;
    Armadura* armaduraEquipada;
    Casco* cascoEquipado;
    Escudo* escudoEquipado;

    int buscarSlotApilable(const Item& item) const;

public:
    Inventario();

    bool estaLleno() const;
    int cantidadSlots() const;
    const std::vector<SlotInventario>& getSlots() const;

    bool agregar(std::unique_ptr<Item> item, int cantidad = 1);
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
};