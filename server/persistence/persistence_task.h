#pragma once

#include <cstdint>
#include <string>

struct PersistenceInventoryItem {
    int slot_id;
    std::string item;
    int cantidad;
    bool equipado;
};

struct PersistenceTask {
    std::string nick;
    std::string raza;
    std::string clase;

    uint16_t mapa_id;
    uint16_t x;
    uint16_t y;
    uint8_t direction;

    uint16_t nivel;
    uint16_t vida;
    uint16_t vida_max;
    uint16_t mana;
    uint16_t mana_max;

    uint32_t experiencia;
    uint32_t oro;

    uint16_t constitucion;
    uint16_t inteligencia;
    uint16_t fuerza;
    uint16_t agilidad;

    std::vector<PersistenceInventoryItem> inventario;
};