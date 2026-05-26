#include <iostream>
#include <stdexcept>

#include "server.h"
#include "config.h"

int main(int argc, char const *argv[]) {

    if (argc != 2) {

        std::cerr
            << "Uso: "
            << argv[0]
            << " <puerto>\n";

        return 1;
    }

    try {

        // cargar configuración TOML
        Config::load("config/server.toml");

        std::string port = argv[1];

        Server server(port.c_str());

        server.run();

    } catch (const std::exception& e) {

        std::cerr
            << "Error: "
            << e.what()
            << "\n";

        return 1;
    }

    return 0;
}