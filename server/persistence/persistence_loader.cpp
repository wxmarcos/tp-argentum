#include "server/persistence/persistence_loader.h"

#include <filesystem>
#include <iostream>
#include <toml++/toml.hpp>

static std::string normalizar_nombre(std::string value) {
    for (char& c : value) {
        c = static_cast<char>(std::tolower(c));
    }
    return value;
}

std::vector<PersistenceTask> PersistenceLoader::load_players(
    const std::string& path) {
    std::vector<PersistenceTask> players;

    if (!std::filesystem::exists(path)) {
        std::cout << "[PersistenceLoader] no existe " << path << "\n";
        return players;
    }

    toml::table table;

    try {
        table = toml::parse_file(path);
    } catch (const toml::parse_error& e) {
        std::cerr << "[PersistenceLoader] error parseando " << path << ": "
                  << e.what() << "\n";
        return players;
    }

    auto* players_table = table["players"].as_table();
    if (!players_table) {
        return players;
    }

    for (auto& [key, value] : *players_table) {
        auto* p = value.as_table();
        if (!p) continue;

        PersistenceTask task;

        task.nick = p->get("nick")
                        ? p->get("nick")->value_or(std::string(key.str()))
                        : std::string(key.str());

        task.raza = normalizar_nombre(
            p->get("raza") ? p->get("raza")->value_or(std::string("humano"))
                           : std::string("humano"));

        task.clase = normalizar_nombre(
            p->get("clase") ? p->get("clase")->value_or(std::string("mago"))
                            : std::string("mago"));

        auto get_i64 = [&](const char* key, int64_t def) -> int64_t {
            auto* node = p->get(key);
            return node ? node->value_or(int64_t{def}) : def;
        };

        task.mapa_id    = static_cast<uint16_t>(get_i64("mapa_id",    1));
        task.x          = static_cast<uint16_t>(get_i64("x",          10));
        task.y          = static_cast<uint16_t>(get_i64("y",          10));
        task.direction  = static_cast<uint8_t> (get_i64("direction",  2));
        task.nivel      = static_cast<uint16_t>(get_i64("nivel",      1));
        task.vida       = static_cast<uint16_t>(get_i64("vida",       1));
        task.vida_max   = static_cast<uint16_t>(get_i64("vida_max",   1));
        task.mana       = static_cast<uint16_t>(get_i64("mana",       0));
        task.mana_max   = static_cast<uint16_t>(get_i64("mana_max",   0));
        task.experiencia= static_cast<uint32_t>(get_i64("experiencia",0));
        task.oro        = static_cast<uint32_t>(get_i64("oro",        0));
        task.constitucion  = static_cast<uint16_t>(get_i64("constitucion",  0));
        task.inteligencia  = static_cast<uint16_t>(get_i64("inteligencia",  0));
        task.fuerza        = static_cast<uint16_t>(get_i64("fuerza",        0));
        task.agilidad      = static_cast<uint16_t>(get_i64("agilidad",      0));
        auto* inventario_table =
            p->get("inventario") ? p->get("inventario")->as_table() : nullptr;

        if (inventario_table) {
            auto* slots_array = inventario_table->get("slot")
                                    ? inventario_table->get("slot")->as_array()
                                    : nullptr;

            if (slots_array) {
                for (auto& elem : *slots_array) {
                    auto* slot_table = elem.as_table();
                    if (!slot_table) {
                        continue;
                    }

                    PersistenceInventoryItem item;

                    item.slot_id = static_cast<int>(
                        slot_table->get("slot_id")
                            ? slot_table->get("slot_id")->value_or<int64_t>(0)
                            : 0);

                    item.item =
                        slot_table->get("item")
                            ? slot_table->get("item")->value_or(std::string(""))
                            : std::string("");

                    item.cantidad = static_cast<int>(
                        slot_table->get("cantidad")
                            ? slot_table->get("cantidad")->value_or<int64_t>(1)
                            : 1);

                    item.equipado =
                        slot_table->get("equipado")
                            ? slot_table->get("equipado")->value_or(false)
                            : false;

                    if (!item.item.empty() && item.cantidad > 0) {
                        task.inventario.push_back(item);
                    }
                }
            }
        }
        players.push_back(task);
    }

    return players;
}
