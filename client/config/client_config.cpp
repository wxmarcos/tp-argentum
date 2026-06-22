#include "config/client_config.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string_view>
#include <toml++/toml.hpp>

namespace fs = std::filesystem;

static constexpr std::string_view PROJECT_NAME = "argentum";

std::string ClientConfig::find_config_file() {
    if (const char* home = std::getenv("HOME")) {
        fs::path user_config =
            fs::path(home) / ".config" / PROJECT_NAME / "client.toml";
        if (fs::exists(user_config)) {
            return user_config.string();
        }
    }

    fs::path repo_config = fs::path("config") / "client.toml";
    if (fs::exists(repo_config)) {
        return repo_config.string();
    }

    return {};
}

template <typename T, typename Table>
void ClientConfig::read_field(const Table& tbl, const std::string& section,
                              const std::string& key, T& out) {
    if (auto value = tbl[section][key].template value<T>()) {
        out = *value;
    }
}

template <typename Table>
void ClientConfig::read_map_files(const Table& tbl) {
    if (auto* mapas = tbl["mapas"].as_table()) {
        for (auto&& [k, v] : *mapas) {
            if (auto s = v.template value<std::string>()) {
                map_files[std::stoi(std::string(k.str()))] = *s;
            }
        }
    }
}

template <typename Table>
void ClientConfig::apply_table(const Table& tbl) {
    read_field(tbl, "server", "host", server_host);
    read_field(tbl, "server", "port", server_port);

    read_field(tbl, "window", "width", window_width);
    read_field(tbl, "window", "height", window_height);
    read_field(tbl, "window", "fullscreen", fullscreen);
    read_field(tbl, "window", "title", window_title);
    read_field(tbl, "window", "hud_panel_fraction", hud_panel_fraction);

    read_field(tbl, "render", "tile_size", tile_size);
    read_field(tbl, "render", "assets_path", assets_path);

    read_field(tbl, "world", "map_width", map_width);
    read_field(tbl, "world", "map_height", map_height);
    read_field(tbl, "world", "map_name", map_name);

    read_map_files(tbl);

    read_field(tbl, "audio", "music_volume", music_volume);
    read_field(tbl, "audio", "effects_volume", effects_volume);

    read_field(tbl, "ui", "font_path", font_path);
    read_field(tbl, "ui", "font_size", font_size);

    read_field(tbl, "character", "nick", character_nick);
    read_field(tbl, "character", "raza", character_raza);
    read_field(tbl, "character", "clase", character_clase);
}

ClientConfig ClientConfig::load() {
    ClientConfig cfg;

    const std::string path = find_config_file();
    if (path.empty()) {
        std::cerr << "[Config] No se encontro client.toml, usando defaults.\n";
        return cfg;
    }

    toml::table tbl;
    try {
        tbl = toml::parse_file(path);
    } catch (const toml::parse_error& err) {
        std::cerr << "[Config] Error parseando " << path << ": "
                  << err.description() << "\n";
        throw;
    }

    cfg.apply_table(tbl);

    std::cout << "[Config] Cargado desde " << path << "\n";
    return cfg;
}

std::string ClientConfig::map_name_for(int id) const {
    auto it = map_files.find(id);
    return it != map_files.end() ? it->second : map_name;
}

int ClientConfig::hud_panel_width() const {
    return static_cast<int>(window_width * hud_panel_fraction);
}

int ClientConfig::game_area_width() const {
    return window_width - hud_panel_width();
}
