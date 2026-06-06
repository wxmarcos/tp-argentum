#include "server/persistence/persistence_loader.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "server/persistence/persistence_record_mapper.h"

std::vector<PersistenceTask> PersistenceLoader::load_players(
    const std::string& path) {
    std::vector<PersistenceTask> players;

    if (!std::filesystem::exists(path)) {
        std::cout << "[PersistenceLoader] no existe " << path << "\n";
        return players;
    }

    std::ifstream in(path, std::ios::binary);

    if (!in) {
        std::cout << "[PersistenceLoader] no se pudo abrir " << path << "\n";
        return players;
    }

    uint32_t count = 0;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    if (!in) {
        std::cerr << "[PersistenceLoader] archivo vacio o corrupto: " << path
                  << "\n";
        return players;
    }

    for (uint32_t i = 0; i < count; ++i) {
        PersistencePlayerRecord record{};

        in.read(reinterpret_cast<char*>(&record), sizeof(record));

        if (!in) {
            std::cerr << "[PersistenceLoader] archivo incompleto: " << path
                      << "\n";
            break;
        }

        players.push_back(PersistenceRecordMapper::from_record(record));
    }

    return players;
}
