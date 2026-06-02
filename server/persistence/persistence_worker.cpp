#include "persistence_worker.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>

PersistenceWorker::PersistenceWorker(
    Queue<PersistenceTask>& queue,
    const std::string& save_directory)
    : queue(queue),
      save_directory(save_directory) {}

static void write_task_toml(std::ofstream& out,
                            const PersistenceTask& task) {
    out << "[players." << task.nick << "]\n";
    out << "nick = \"" << task.nick << "\"\n";
    out << "raza = \"" << task.raza << "\"\n";
    out << "clase = \"" << task.clase << "\"\n";
    out << "mapa_id = " << task.mapa_id << "\n";
    out << "x = " << task.x << "\n";
    out << "y = " << task.y << "\n";
    out << "direction = " << int(task.direction) << "\n";
    out << "nivel = " << task.nivel << "\n";
    out << "vida = " << task.vida << "\n";
    out << "vida_max = " << task.vida_max << "\n";
    out << "mana = " << task.mana << "\n";
    out << "mana_max = " << task.mana_max << "\n\n";
}

static void save_all_players(
    const std::filesystem::path& file_path,
    const std::map<std::string, PersistenceTask>& players) {
    std::ofstream out(file_path, std::ios::trunc);

    if (!out) {
        std::cerr << "[PersistenceWorker] no se pudo abrir "
                  << file_path << "\n";
        return;
    }

    for (const auto& [nick, task] : players) {
        write_task_toml(out, task);
    }
}

void PersistenceWorker::run() {
    try {
        std::filesystem::create_directories(save_directory);
    } catch (const std::exception& ex) {
        std::cerr << "[PersistenceWorker] no se pudo crear directorio: "
                  << ex.what() << "\n";
    }

    std::filesystem::path file_path =
        std::filesystem::path(save_directory) / "players.toml";

    std::map<std::string, PersistenceTask> players;

    while (true) {
        try {
            PersistenceTask task = queue.pop();

            players[task.nick] = task;

            save_all_players(file_path, players);

        } catch (const ClosedQueue&) {
            break;
        } catch (const std::exception& ex) {
            std::cerr << "[PersistenceWorker] error: "
                      << ex.what() << "\n";
        }
    }
}