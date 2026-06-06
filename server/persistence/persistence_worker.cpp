#include "server/persistence/persistence_worker.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>

#include "server/persistence/persistence_loader.h"
#include "server/persistence/persistence_record_mapper.h"

PersistenceWorker::PersistenceWorker(Queue<PersistenceTask>& queue,
                                     const std::string& save_file_path):
    queue(queue),
    save_file_path(save_file_path) {}

static void save_all_players(
    const std::filesystem::path& file_path,
    const std::map<std::string, PersistenceTask>& players) {
    std::ofstream out(file_path, std::ios::binary | std::ios::trunc);

    if (!out) {
        std::cout << "[PersistenceWorker] no se pudo abrir " << file_path
                  << "\n";
        return;
    }
    std::cout << "[PersistenceWorker] guardando " << players.size()
              << " jugadores en " << file_path << "\n";
    uint32_t count = static_cast<uint32_t>(players.size());
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& [nick, task] : players) {
        PersistencePlayerRecord record =
            PersistenceRecordMapper::to_record(task);

        out.write(reinterpret_cast<const char*>(&record), sizeof(record));
    }
}

void PersistenceWorker::run() {
    std::filesystem::path file_path(save_file_path);
    std::filesystem::path directory = file_path.parent_path();

    if (!directory.empty()) {
        try {
            std::filesystem::create_directories(directory);
        } catch (const std::exception& ex) {
            std::cout << "[PersistenceWorker] no se pudo crear directorio: "
                      << ex.what() << "\n";
        }
    }

    std::map<std::string, PersistenceTask> players;

    try {
        std::cout << "[PersistenceLoader] leyendo " << file_path << "\n";

        auto loaded_players = PersistenceLoader::load_players(save_file_path);

        for (const auto& player : loaded_players) {
            std::cout << "[PersistenceWorker] cargado " << player.nick
                      << " inventario=" << player.inventario.size() << "\n";

            players[player.nick] = player;
        }

    } catch (const std::exception& ex) {
        std::cout
            << "[PersistenceWorker] no se pudo cargar persistencia inicial: "
            << ex.what() << "\n";
    }

    while (true) {
        try {
            PersistenceTask task = queue.pop();

            players[task.nick] = task;

            save_all_players(file_path, players);

        } catch (const ClosedQueue&) {
            break;
        } catch (const std::exception& ex) {
            std::cout << "[PersistenceWorker] error: " << ex.what() << "\n";
        }
    }
}
