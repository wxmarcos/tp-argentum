#include <atomic>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdexcept>

#include "game/config.h"
#include "server/server.h"

int main(int argc, char const* argv[]) {
    srand(static_cast<unsigned int>(time(nullptr)));
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <puerto>\n";
        return 1;
    }

    try {
        std::string port = argv[1];
        Config game_config("config.toml");

        Server server(port.c_str(), game_config);

        server.start();

        std::cout << "[Server] Servidor corriendo.\n";
        std::cout << "Presione 'q' + ENTER para cerrar el servidor.\n";

        char c;
        while (std::cin >> c) {
            if (c == 'q' || c == 'Q') {
                break;
            }
        }

        std::cout << "[Server] Deteniendo servidor...\n";

        server.stop();
        server.join();

        std::cout << "[Server] Servidor detenido correctamente.\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
