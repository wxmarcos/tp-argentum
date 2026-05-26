#include "config.h"

#include <iostream>

#include <toml++/toml.h>

int Config::MAP_WIDTH = 100;
int Config::MAP_HEIGHT = 100;

int Config::TICKS_PER_SECOND = 30;

int Config::MAX_CLIENTS = 100;

void Config::load(const std::string& filename) {

    try {

        auto config = toml::parse_file(filename);

        auto server = config["server"];

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