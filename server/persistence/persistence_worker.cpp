#include "persistence_worker.h"

#include <filesystem>
#include <fstream>
#include <iostream>

PersistenceWorker::PersistenceWorker(
    Queue<PersistenceTask>& queue,
    const std::string& save_directory)
    : queue(queue),
      save_directory(save_directory) {}

void PersistenceWorker::run() {
    try {
        std::filesystem::create_directories(save_directory);
    } catch (const std::exception& ex) {
        std::cerr << "[PersistenceWorker] no se pudo crear directorio: "
                  << ex.what() << "\n";
    }

    while (true) {
        try {
            PersistenceTask task = queue.pop();

            std::filesystem::path file_path =
                std::filesystem::path(save_directory) /
                (task.nick + ".txt");

            std::ofstream out(file_path, std::ios::app);
            if (!out) {
                std::cerr << "[PersistenceWorker] no se pudo abrir "
                          << file_path << "\n";
                continue;
            }

            out << task.nick << ','
                << task.raza << ','
                << task.clase << ','
                << task.mapa_id << ','
                << task.x << ','
                << task.y << ','
                << int(task.direction) << ','
                << task.nivel << ','
                << task.vida << ','
                << task.vida_max << ','
                << task.mana << ','
                << task.mana_max << '\n';

        } catch (const ClosedQueue&) {
            break;
        } catch (const std::exception& ex) {
            std::cerr << "[PersistenceWorker] error: "
                      << ex.what() << "\n";
        }
    }
}
