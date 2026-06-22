#include <iostream>

#include "config/client_config.h"
#include "ui/client_app.h"

int main(int argc, const char* argv[]) {
    ClientConfig config = ClientConfig::load();
    if (argc > 1) config.character_nick = argv[1];
    if (argc > 2) config.character_raza = argv[2];
    if (argc > 3) config.character_clase = argv[3];
    ClientApp app(std::move(config));
    return app.run();
}
