#include "server/persistence/persistence_worker.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>

#include "server/persistence/persistence_loader.h"

PersistenceWorker::PersistenceWorker(Queue<PersistenceTask>& queue,
                                     const std::string& save_file_path):
    queue(queue),
    save_file_path(save_file_path) {}

static void write_task_toml(std::ofstream& out, const PersistenceTask& task) {
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
    out << "mana_max = " << task.mana_max << "\n";
    out << "experiencia = " << task.experiencia << "\n";
    out << "oro = " << task.oro << "\n";
    out << "constitucion = " << task.constitucion << "\n";
    out << "inteligencia = " << task.inteligencia << "\n";
    out << "fuerza = " << task.fuerza << "\n";
    out << "agilidad = " << task.agilidad << "\n\n";

    for (const auto& item : task.inventario) {
        out << "[[players." << task.nick << ".inventario.slot]]\n";
        out << "slot_id = " << item.slot_id << "\n";
        out << "item = \"" << item.item << "\"\n";
        out << "cantidad = " << item.cantidad << "\n";
        out << "equipado = " << (item.equipado ? "true" : "false") << "\n\n";
    }
}

static void save_all_players(
    const std::filesystem::path& file_path,
    const std::map<std::string, PersistenceTask>& players) {
    std::ofstream out(file_path, std::ios::trunc);

    if (!out) {
        std::cout << "[PersistenceWorker] no se pudo abrir " << file_path
                  << "\n";
        return;
    }

    for (const auto& [nick, task] : players) {
        write_task_toml(out, task);
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
        auto loaded_players = PersistenceLoader::load_players(save_file_path);

        for (const auto& player : loaded_players) {
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
