#pragma once

#include <cstdint>
#include <string>

struct PersistenceTask {
    std::string nick;

    std::string raza;
    std::string clase;

    uint16_t mapa_id = 0;

    uint16_t x = 0;
    uint16_t y = 0;

    uint8_t direction = 0;

    uint16_t nivel = 1;

    uint16_t vida = 0;
    uint16_t vida_max = 0;

    uint16_t mana = 0;
    uint16_t mana_max = 0;
};