#ifndef CLIENT_RENDER_TILE_CATALOG_H
#define CLIENT_RENDER_TILE_CATALOG_H

#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>

#include <SDL2/SDL.h>

#include "render/texture_manager.h"

using TileId = uint16_t;

struct TileDef {
    std::string atlas_key;
    SDL_Rect src;
};

class TileCatalog {
    private:
    TextureManager textures;
    std::unordered_map<TileId, TileDef> tiles;

    void load_tsx(const std::filesystem::path& tsx_path, TileId firstgid);

    bool parse_tsx_header(const std::filesystem::path& tsx_path, int& columns,
                          int& tilewidth, int& tileheight, int& tilecount,
                          std::string& png_rel) const;

    void register_tiles(const std::string& key, TileId firstgid, int columns,
                        int tilewidth, int tileheight, int tilecount);
                        
    std::string attr(const std::string& line, const std::string& name) const;

    public:
    TileCatalog(SDL_Renderer* renderer,
                const std::filesystem::path& assets_root);

    void add_tileset(const std::filesystem::path& tsx_path, TileId firstgid);

    void add_inline_tileset(const std::string& name,
                            const std::filesystem::path& png_path,
                            TileId firstgid, int columns, int tilewidth,
                            int tileheight, int tilecount);

    SDL_Texture* texture_for(TileId id) const;

    SDL_Rect src_for(TileId id) const;

    bool has(TileId id) const;
};

#endif