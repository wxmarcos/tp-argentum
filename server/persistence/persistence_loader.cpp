#include "server/persistence/persistence_loader.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>

#include "server/persistence/persistence_index_record.h"
#include "server/persistence/persistence_record.h"
#include "server/persistence/persistence_record_mapper.h"

std::unordered_map<std::string, uint64_t> PersistenceLoader::load_index(
    const std::string& path) {
    std::unordered_map<std::string, uint64_t> index;

    if (!std::filesystem::exists(path)) {
        std::cout << "[PersistenceLoader] no existe indice " << path << "\n";
        return index;
    }

    std::ifstream in(path, std::ios::binary);

    if (!in) {
        std::cout << "[PersistenceLoader] no se pudo abrir indice " << path
                  << "\n";
        return index;
    }

    PersistenceIndexRecord record{};

    while (in.read(reinterpret_cast<char*>(&record), sizeof(record))) {
        std::string nick(record.nick);

        if (!nick.empty()) {
            index[nick] = record.offset;
        }
    }

    std::cout << "[PersistenceLoader] indice cargado: " << index.size()
              << " jugadores\n";

    return index;
}
std::optional<PersistenceTask> PersistenceLoader::load_player_by_nick(
    const std::string& players_path,
    const std::string& index_path,
    const std::string& nick) {

    std::cout << "[PersistenceLoader] players_path=" << players_path << "\n";
    std::cout << "[PersistenceLoader] index_path=" << index_path << "\n";
    std::cout << "[PersistenceLoader] buscando nick=" << nick << "\n";

    auto index = load_index(index_path);

    std::cout << "[PersistenceLoader] index size=" << index.size() << "\n";

    for (const auto& [index_nick, offset] : index) {
        std::cout << "[PersistenceLoader] index nick=["
                  << index_nick << "] offset=" << offset << "\n";
    }

    auto it = index.find(nick);
    if (it == index.end()) {
        std::cout << "[PersistenceLoader] nick no encontrado en indice\n";
        return std::nullopt;
    }

    std::ifstream in(players_path, std::ios::binary);
    if (!in) {
        std::cout << "[PersistenceLoader] no se pudo abrir players file\n";
        return std::nullopt;
    }

    in.seekg(static_cast<std::streamoff>(it->second), std::ios::beg);

    PersistencePlayerRecord record{};
    in.read(reinterpret_cast<char*>(&record), sizeof(record));

    if (!in) {
        std::cout << "[PersistenceLoader] no se pudo leer record en offset "
                  << it->second << "\n";
        return std::nullopt;
    }

    PersistenceTask task = PersistenceRecordMapper::from_record(record);

    std::cout << "[PersistenceLoader] record leido nick=["
              << task.nick << "] raza=[" << task.raza
              << "] clase=[" << task.clase << "] mapa="
              << task.mapa_id << " x=" << task.x << " y=" << task.y << "\n";

    return task;
}
