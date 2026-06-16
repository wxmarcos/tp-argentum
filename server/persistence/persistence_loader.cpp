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
        return index;
    }

    std::ifstream in(path, std::ios::binary);

    if (!in) {
        return index;
    }

    PersistenceIndexRecord record{};

    while (in.read(reinterpret_cast<char*>(&record), sizeof(record))) {
        std::string nick(record.nick);

        if (!nick.empty()) {
            index[nick] = record.offset;
        }
    }

    return index;
}
std::optional<PersistenceTask> PersistenceLoader::load_player_by_nick(
    const std::string& players_path,
    const std::string& index_path,
    const std::string& nick) {

    auto index = load_index(index_path);

    auto it = index.find(nick);
    if (it == index.end()) {
        return std::nullopt;
    }

    std::ifstream in(players_path, std::ios::binary);
    if (!in) {
        return std::nullopt;
    }

    in.seekg(static_cast<std::streamoff>(it->second), std::ios::beg);

    PersistencePlayerRecord record{};
    in.read(reinterpret_cast<char*>(&record), sizeof(record));

    if (!in) {
        return std::nullopt;
    }

    PersistenceTask task = PersistenceRecordMapper::from_record(record);


    return task;
}
