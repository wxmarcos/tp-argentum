#include <iostream>

#include "config/client_config.h"
#include "ui/client_app.h"

int main() {
    ClientConfig config = ClientConfig::load();
    ClientApp app(std::move(config));
    return app.run();
}
