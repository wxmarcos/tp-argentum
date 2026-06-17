// tools/create_test_players.cpp
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cstring>

#include "server/persistence/persistence_index_record.h"
#include "server/persistence/persistence_record_mapper.h"

static void copy_string(char* dest, std::size_t size, const std::string& src) {
    std::memset(dest, 0, size);
    std::strncpy(dest, src.c_str(), size - 1);
}

static void write_index(std::ofstream& out, const std::string& nick, uint64_t offset) {
    PersistenceIndexRecord record{};
    copy_string(record.nick, PERSISTENCE_INDEX_NICK_SIZE, nick);
    record.offset = offset;
    out.write(reinterpret_cast<const char*>(&record), sizeof(record));
}

static PersistenceTask make_player(const std::string& nick) {
    PersistenceTask task;

    task.nick = nick;
    task.raza = "humano";
    task.clase = "mago";

    task.mapa_id = 1;
    task.x = 25;
    task.y = 25;
    task.direction = 2;

    task.nivel = 10;
    task.vida = 100;
    task.vida_max = 100;
    task.mana = 80;
    task.mana_max = 80;

    task.experiencia = 1200;
    task.oro = 500;

    task.constitucion = 18;
    task.inteligencia = 20;
    task.fuerza = 16;
    task.agilidad = 15;

    task.inventario.push_back({0, "Vara de fresno", 1, true});
    task.inventario.push_back({1, "Tunica azul", 1, true});
    task.inventario.push_back({2, "Pocion de mana", 10, false});

    return task;
}

int main() {
    std::filesystem::create_directories("data");

    std::ofstream players("data/players.bin",
                          std::ios::binary | std::ios::trunc);
    std::ofstream index("data/index.bin",
                        std::ios::binary | std::ios::trunc);

    uint64_t offset = 0;

    PersistenceTask marcos = make_player("MarcosTest");
    PersistencePlayerRecord recordMarcos =
        PersistenceRecordMapper::to_record(marcos);

    players.write(reinterpret_cast<const char*>(&recordMarcos),
                  sizeof(recordMarcos));
    write_index(index, marcos.nick, offset);

    std::cout << "nick=" << marcos.nick
              << " offset=" << offset << "\n";

    offset += sizeof(recordMarcos);

    PersistenceTask pepe = make_player("Pepe");
    pepe.raza = "elfo";
    pepe.clase = "guerrero";
    pepe.x = 30;
    pepe.y = 30;
    pepe.inventario.clear();
    pepe.inventario.push_back({0, "Espada", 1, true});
    pepe.inventario.push_back({1, "Escudo de hierro", 1, true});

    PersistencePlayerRecord recordPepe =
        PersistenceRecordMapper::to_record(pepe);

    players.write(reinterpret_cast<const char*>(&recordPepe),
                  sizeof(recordPepe));
    write_index(index, pepe.nick, offset);

    std::cout << "nick=" << pepe.nick
              << " offset=" << offset << "\n";

    std::cout << "Creado data/players.bin\n";
    std::cout << "Creado data/index.bin\n";
    std::cout << "record size=" << sizeof(PersistencePlayerRecord) << "\n";
            std::cout << "Creado data/players.bin\n";
    std::cout << "Creado data/index.bin\n";
    std::cout << "record size=" << sizeof(PersistencePlayerRecord) << "\n";
    std::cout << "index record size=" << sizeof(PersistenceIndexRecord) << "\n";

    players.flush();
    index.flush();

    players.close();
    index.close();

    return 0;
}