#pragma once

#include <string>

class Mapa;

class TmxLoader {
public:
    static void cargarColisiones(const std::string& rutaTmx, Mapa& mapa);
};
