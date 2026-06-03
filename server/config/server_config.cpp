#include "server_config.h"

#include <iostream>

#include <toml++/toml.h>

int ServerConfig::MAP_WIDTH = 100;
int ServerConfig::MAP_HEIGHT = 100;

int ServerConfig::TICKS_PER_SECOND = 30;

int ServerConfig::MAX_CLIENTS = 100;

void ServerConfig::load(const std::string& filename) {

    try {

        auto serverconfig =
            toml::parse_file(filename);

        auto server =
            serverconfig["server"];

        MAP_WIDTH =
            server["map_width"].value_or(100);

        MAP_HEIGHT =
            server["map_height"].value_or(100);

        TICKS_PER_SECOND =
            server["ticks_per_second"].value_or(30);

        MAX_CLIENTS =
            server["max_clients"].value_or(100);

    } catch (const std::exception& e) {

        std::cerr
            << "Error cargando config TOML: "
            << e.what()
            << "\n";

        throw;
    }
}