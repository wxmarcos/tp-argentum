#pragma once

#include <cstddef>
#include <cstdint>

constexpr std::size_t PERSISTENCE_NICK_SIZE = 32;
constexpr std::size_t PERSISTENCE_RAZA_SIZE = 16;
constexpr std::size_t PERSISTENCE_CLASE_SIZE = 16;
constexpr std::size_t PERSISTENCE_ITEM_SIZE = 32;
constexpr std::size_t PERSISTENCE_MAX_INVENTORY = 30;
constexpr std::size_t PERSISTENCE_MAX_BANK_ITEMS = 100;

struct PersistenceInventoryRecord {
    int32_t slot_id;
    char item[PERSISTENCE_ITEM_SIZE];
    int32_t cantidad;
    uint8_t equipado;
};

struct PersistencePlayerRecord {
    char nick[PERSISTENCE_NICK_SIZE];
    char raza[PERSISTENCE_RAZA_SIZE];
    char clase[PERSISTENCE_CLASE_SIZE];

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

    uint32_t inventario_count;
    PersistenceInventoryRecord inventario[PERSISTENCE_MAX_INVENTORY];
    uint32_t banco_oro;
    uint32_t banco_items_count;
    PersistenceInventoryRecord banco_items[PERSISTENCE_MAX_BANK_ITEMS];
};
