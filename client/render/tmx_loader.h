#ifndef CLIENT_RENDER_TMX_LOADER_H
#define CLIENT_RENDER_TMX_LOADER_H

#include <filesystem>
#include <memory>

#include "render/map_data.h"
#include "render/tile_catalog.h"

// TmxLoader parsea un archivo .tmx y produce:
//   - Un TileCatalog con todos los tilesets cargados
//   - Un MapData con las capas de tiles y las colisiones
struct LoadedMap {
    std::unique_ptr<TileCatalog> catalog;
    std::unique_ptr<MapData>     map;
};

LoadedMap load_tmx(const std::filesystem::path& tmx_path,
                   SDL_Renderer* renderer);

#endif