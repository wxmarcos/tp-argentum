#ifndef CLIENT_RENDER_TMX_LOADER_H
#define CLIENT_RENDER_TMX_LOADER_H

#include <filesystem>
#include <iosfwd>
#include <memory>
#include <string>

#include "render/map/map_data.h"
#include "render/map/tile_catalog.h"

struct LoadedMap {
    std::unique_ptr<TileCatalog> catalog;
    std::unique_ptr<MapData> map;
};

class TmxLoader {
public:
    LoadedMap load(const std::filesystem::path& tmx_path,
                   SDL_Renderer* renderer);

private:
    std::string attr(const std::string& line, const std::string& name) const;

    void read_map_header(const std::filesystem::path& tmx_path, int& map_w,
                         int& map_h, int& tile_size) const;

    int count_layers(const std::filesystem::path& tmx_path) const;

    void parse_body(std::ifstream& f, const std::filesystem::path& tmx_dir,
                    TileCatalog& catalog, MapData& map, int tile_size);

    void parse_external_tileset(const std::string& line,
                                const std::filesystem::path& tmx_dir,
                                TileCatalog& catalog);

    void parse_inline_tileset(std::ifstream& f, const std::string& line,
                              const std::filesystem::path& tmx_dir,
                              TileCatalog& catalog);

    void parse_csv_layer(const std::string& csv, MapData& map,
                         int layer_idx) const;

    void parse_collision_layer(std::ifstream& f, MapData& map,
                               int tile_size) const;
};

#endif
