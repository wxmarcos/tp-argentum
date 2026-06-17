#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "server/persistence/persistence_record.h"

static void copy_string(char* dest, std::size_t size, const std::string& src) {
    std::memset(dest, 0, size);
    std::strncpy(dest, src.c_str(), size - 1);
}

static void set_item(PersistencePlayerRecord& player,
                     uint32_t index,
                     uint16_t slot_id,
                     const std::string& item,
                     uint16_t cantidad,
                     bool equipado) {
    auto& slot = player.inventario[index];

    slot.slot_id = slot_id;
    copy_string(slot.item, PERSISTENCE_ITEM_SIZE, item);
    slot.cantidad = cantidad;
    slot.equipado = equipado ? 1 : 0;
}

int main() {
    std::filesystem::create_directories("data");

    PersistencePlayerRecord player{};

    copy_string(player.nick, PERSISTENCE_NICK_SIZE, "InventoryTest");
    copy_string(player.raza, PERSISTENCE_RAZA_SIZE, "humano");
    copy_string(player.clase, PERSISTENCE_CLASE_SIZE, "mago");

    player.mapa_id = 1;
    player.x = 10;
    player.y = 10;
    player.direction = 2;

    player.nivel = 10;
    player.vida = 240;
    player.vida_max = 240;
    player.mana = 450;
    player.mana_max = 450;

    player.experiencia = 0;
    player.oro = 5000;

    player.constitucion = 300;
    player.inteligencia = 300;
    player.fuerza = 300;
    player.agilidad = 300;

    player.inventario_count = 4;

    set_item(player, 0, 0, "Vara de fresno", 1, true);
    set_item(player, 1, 1, "Túnica azul", 1, true);
    set_item(player, 2, 2, "Sombrero mágico", 1, true);
    set_item(player, 3, 3, "Poción de mana", 10, false);

    std::ofstream out("data/players.bin", std::ios::binary | std::ios::trunc);

    uint32_t count = 1;
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    out.write(reinterpret_cast<const char*>(&player), sizeof(player));

    std::cout << "Archivo creado: data/players.bin\n";
    std::cout << "sizeof(PersistencePlayerRecord)="
              << sizeof(PersistencePlayerRecord)
              << "\n";
    std::cout << "inventario_count=" << player.inventario_count << "\n";

    return 0;
}