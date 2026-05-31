#include "config/client_config.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>

#include <toml++/toml.hpp>

namespace {

namespace fs = std::filesystem;

constexpr const char* PROJECT_NAME = "argentum";


fs::path find_config_file() {
    if (const char* home = std::getenv("HOME")) {
        fs::path user_config = fs::path(home) / ".config" / PROJECT_NAME /
                                "client.toml";
        if (fs::exists(user_config)) {
            return user_config;
        }
    }

    fs::path repo_config = fs::path("config") / "client.toml";
    if (fs::exists(repo_config)) {
        return repo_config;
    }

    return {};
}

template <typename T>
void read_field(const toml::table& tbl, const std::string& section,
                const std::string& key, T& out) {
    if (auto value = tbl[section][key].value<T>()) {
        out = *value;
    }
}
}

ClientConfig ClientConfig::load() {
    ClientConfig cfg;

    fs::path path = find_config_file();
    if (path.empty()) {
        std::cerr << "[Config] No se encontro client.toml, usando defaults.\n";
        return cfg;
    }

    toml::table tbl;
    try {
        tbl = toml::parse_file(path.string());
    } catch (const toml::parse_error& err) {
        std::cerr << "[Config] Error parseando " << path << ": "
                  << err.description() << "\n";
        throw;
    }

    read_field(tbl, "server", "host", cfg.server_host);
    read_field(tbl, "server", "port", cfg.server_port);

    read_field(tbl, "window", "width", cfg.window_width);
    read_field(tbl, "window", "height", cfg.window_height);
    read_field(tbl, "window", "fullscreen", cfg.fullscreen);
    read_field(tbl, "window", "title", cfg.window_title);

    read_field(tbl, "render", "tile_size", cfg.tile_size);

    read_field(tbl, "world", "map_width", cfg.map_width);
    read_field(tbl, "world", "map_height", cfg.map_height);

    read_field(tbl, "audio", "music_volume", cfg.music_volume);
    read_field(tbl, "audio", "effects_volume", cfg.effects_volume);

    read_field(tbl, "character", "nick", cfg.character_nick);
    read_field(tbl, "character", "raza", cfg.character_raza);
    read_field(tbl, "character", "clase", cfg.character_clase);

    std::cout << "[Config] Cargado desde " << path << "\n";
    return cfg;
}
