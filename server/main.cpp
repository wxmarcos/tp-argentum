#include <iostream>
#include <stdexcept>
#include "common/network/socket.h"

int main() {
    try {
        Socket listener("8080");
        std::cout << "Servidor escuchando en puerto 8080...\n";

        while (true) {
            Socket client = listener.accept();
            std::cout << "Cliente conectado\n";
            char buffer[1024];

            while (true) {
                int received = client.recvsome(buffer, sizeof(buffer) - 1);
                if (received == 0) {
                    std::cout << "Cliente desconectado\n";
                    break;
                }

                buffer[received] = '\0';
                std::cout << buffer;
                std::cout.flush();
            }
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
