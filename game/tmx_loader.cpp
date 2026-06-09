#include "game/tmx_loader.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

#include "game/mapa.h"

static constexpr int TILE_SIZE = 32;

static std::string leerArchivo(const std::string& ruta) {
    std::ifstream file(ruta);
    if (!file) {
        throw std::runtime_error("No se pudo abrir TMX: " + ruta);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static std::string extraerObjectGroupColision(const std::string& xml) {
    std::regex objectGroupRegex(
        R"REGEX(<objectgroup[^>]*name="Colision"[^>]*>([\s\S]*?)</objectgroup>)REGEX");

    std::smatch match;
    if (std::regex_search(xml, match, objectGroupRegex)) {
        return match[1].str();
    }

    return "";
}

void TmxLoader::cargarColisiones(const std::string& rutaTmx, Mapa& mapa) {
    std::cout << "[TmxLoader] leyendo colisiones por objetos: "
              << rutaTmx << "\n";

    std::string xml = leerArchivo(rutaTmx);
    std::string objectGroup = extraerObjectGroupColision(xml);

    if (objectGroup.empty()) {
        throw std::runtime_error("No se encontro objectgroup Colision en: " +
                                 rutaTmx);
    }

    std::regex objectRegex(
        R"REGEX(<object[^>]*x="([^"]+)"[^>]*y="([^"]+)"(?:[^>]*width="([^"]+)")?(?:[^>]*height="([^"]+)")?[^>]*/>)REGEX");

    auto begin = std::sregex_iterator(objectGroup.begin(), objectGroup.end(),
                                      objectRegex);
    auto end = std::sregex_iterator();

    int objetos = 0;
    int tilesBloqueados = 0;

    for (auto it = begin; it != end; ++it) {
        double x = std::stod((*it)[1].str());
        double y = std::stod((*it)[2].str());

        double w = 1.0;
        double h = 1.0;

        if ((*it)[3].matched) {
            w = std::stod((*it)[3].str());
        }

        if ((*it)[4].matched) {
            h = std::stod((*it)[4].str());
        }

        int tileXInicio = static_cast<int>(std::floor(x / TILE_SIZE));
        int tileYInicio = static_cast<int>(std::floor(y / TILE_SIZE));
        
        int tileXFin = static_cast<int>(std::floor((x + w - 1) / TILE_SIZE));
        int tileYFin = static_cast<int>(std::floor((y + h - 1) / TILE_SIZE));

        for (int ty = tileYInicio; ty <= tileYFin; ty++) {
            for (int tx = tileXInicio; tx <= tileXFin; tx++) {
                double centroX = tx * TILE_SIZE + TILE_SIZE / 2.0;
                double centroY = ty * TILE_SIZE + TILE_SIZE / 2.0;

                bool centroDentro =
                    centroX >= x && centroX <= x + w &&
                    centroY >= y && centroY <= y + h;

                if (centroDentro && mapa.esPosicionValida(tx, ty)) {
                    mapa.setTile(tx, ty, TipoTile::PARED, false);
                    tilesBloqueados++;
                }
            }
        }

        objetos++;
    }

    std::cout << "[TmxLoader] objectgroup Colision: objetos="
              << objetos
              << " tiles_bloqueados="
              << tilesBloqueados
              << "\n";
    
}