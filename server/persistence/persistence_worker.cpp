#include "server/persistence/persistence_worker.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include "server/persistence/persistence_index_record.h"
#include "server/persistence/persistence_loader.h"
#include "server/persistence/persistence_record_mapper.h"

PersistenceWorker::PersistenceWorker(Queue<PersistenceTask>& queue,
                                     const std::string& save_file_path):
    queue(queue),
    save_file_path(save_file_path) {}

static void copy_string(char* dest, std::size_t size, const std::string& src) {
    std::memset(dest, 0, size);
    std::strncpy(dest, src.c_str(), size - 1);
}

static void append_index_record(const std::filesystem::path& index_path,
                                const std::string& nick,
                                uint64_t offset) {
    std::ofstream out(index_path, std::ios::binary | std::ios::app);

    if (!out) {
        return;
    }

    PersistenceIndexRecord record{};
    copy_string(record.nick, PERSISTENCE_INDEX_NICK_SIZE, nick);
    record.offset = offset;

    out.write(reinterpret_cast<const char*>(&record), sizeof(record));
}

static void write_player_record_at(const std::filesystem::path& players_path,
                                   uint64_t offset,
                                   const PersistenceTask& task) {
    std::fstream file(players_path,
                      std::ios::binary | std::ios::in | std::ios::out);

    if (!file) {
        return;
    }

    PersistencePlayerRecord record =
        PersistenceRecordMapper::to_record(task);

    file.seekp(static_cast<std::streamoff>(offset), std::ios::beg);
    file.write(reinterpret_cast<const char*>(&record), sizeof(record));
}


static uint64_t append_player_record(const std::filesystem::path& players_path,
                                     const PersistenceTask& task) {
    std::fstream file(players_path,
                      std::ios::binary | std::ios::in | std::ios::out);

    if (!file) {
        std::ofstream create(players_path, std::ios::binary);
        create.close();

        file.open(players_path,
                  std::ios::binary | std::ios::in | std::ios::out);
    }

    if (!file) {
        return 0;
    }

    file.seekp(0, std::ios::end);

    uint64_t offset =
        static_cast<uint64_t>(file.tellp());

    PersistencePlayerRecord record =
        PersistenceRecordMapper::to_record(task);

    file.write(reinterpret_cast<const char*>(&record), sizeof(record));

    std::cout << "[PersistenceWorker] append record nick="
              << task.nick << " offset=" << offset
              << " size=" << sizeof(record) << "\n";

    return offset;
}

void PersistenceWorker::run() {
    std::filesystem::path players_path(save_file_path);
    std::filesystem::path index_path = players_path.parent_path() / "index.bin";
    std::filesystem::path directory = players_path.parent_path();

    if (!directory.empty()) {
        try {
            std::filesystem::create_directories(directory);
        } catch (const std::exception& ex) {
            std::cout << "[PersistenceWorker] no se pudo crear directorio: "
                      << ex.what() << "\n";
        }
    }

    std::unordered_map<std::string, uint64_t> index =
        PersistenceLoader::load_index(index_path.string());

    while (true) {
        try {
            PersistenceTask task = queue.pop();

            auto it = index.find(task.nick);

            if (it == index.end()) {
                uint64_t offset = append_player_record(players_path, task);

                index[task.nick] = offset;
                append_index_record(index_path, task.nick, offset);

                std::cout << "[PersistenceWorker] nuevo jugador "
                          << task.nick << " offset=" << offset << "\n";
            } else {
                write_player_record_at(players_path, it->second, task);

                std::cout << "[PersistenceWorker] actualizado jugador "
                          << task.nick << " offset=" << it->second << "\n";
            }

        } catch (const ClosedQueue&) {
            break;
        } catch (const std::exception& ex) {
            std::cout << "[PersistenceWorker] error: " << ex.what() << "\n";
        }
    }
}