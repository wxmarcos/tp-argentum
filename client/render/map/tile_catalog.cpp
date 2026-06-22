#include "render/map/tile_catalog.h"

#include <fstream>
#include <iostream>

TileCatalog::TileCatalog(SDL_Renderer* renderer,
                         const std::filesystem::path& assets_root):
    textures(renderer, assets_root) {}

std::string TileCatalog::attr(const std::string& line,
                              const std::string& name) const {
    std::string pat = name + "=\"";
    auto pos = line.find(pat);
    if (pos == std::string::npos) return {};
    pos += pat.size();
    auto end = line.find('"', pos);
    return line.substr(pos, end - pos);
}

bool TileCatalog::parse_tsx_header(const std::filesystem::path& tsx_path,
                                   int& columns, int& tilewidth,
                                   int& tileheight, int& tilecount,
                                   std::string& png_rel) const {
    std::ifstream f(tsx_path);
    if (!f.is_open()) return false;

    std::string line;
    while (std::getline(f, line)) {
        if (line.find("<tileset") != std::string::npos) {
            auto c = attr(line, "columns");
            auto tw = attr(line, "tilewidth");
            auto th = attr(line, "tileheight");
            auto tc = attr(line, "tilecount");
            if (!c.empty()) columns = std::stoi(c);
            if (!tw.empty()) tilewidth = std::stoi(tw);
            if (!th.empty()) tileheight = std::stoi(th);
            if (!tc.empty()) tilecount = std::stoi(tc);
        }
        if (line.find("<image") != std::string::npos) {
            png_rel = attr(line, "source");
        }
    }
    return true;
}

void TileCatalog::register_tiles(const std::string& key, TileId firstgid,
                                 int columns, int tilewidth, int tileheight,
                                 int tilecount) {
    for (int i = 0; i < tilecount; ++i) {
        int col = i % columns;
        int row = i / columns;
        TileId gid = static_cast<TileId>(firstgid + i);
        tiles[gid] = TileDef{key, SDL_Rect{col * tilewidth, row * tileheight,
                                           tilewidth, tileheight}};
    }
}

void TileCatalog::load_tsx(const std::filesystem::path& tsx_path,
                           TileId firstgid) {
    int columns = 0;
    int tilewidth = 32;
    int tileheight = 32;
    int tilecount = 0;
    std::string png_rel;

    if (!parse_tsx_header(tsx_path, columns, tilewidth, tileheight, tilecount,
                          png_rel)) {
        std::cerr << "[TileCatalog] No se pudo abrir: " << tsx_path << "\n";
        return;
    }

    if (png_rel.empty() || columns == 0 || tilecount == 0) {
        std::cerr << "[TileCatalog] TSX incompleto: " << tsx_path << "\n";
        return;
    }

    std::filesystem::path png_abs =
        (tsx_path.parent_path() / png_rel).lexically_normal();
    std::string key = png_abs.string();

    textures.try_load(key, png_abs.string());
    register_tiles(key, firstgid, columns, tilewidth, tileheight, tilecount);

    std::cout << "[TileCatalog] " << tsx_path.filename()
              << " firstgid=" << firstgid << " tiles=" << tilecount << "\n";
}

void TileCatalog::add_tileset(const std::filesystem::path& tsx_path,
                              TileId firstgid) {
    load_tsx(tsx_path, firstgid);
}

void TileCatalog::add_inline_tileset(const std::string& name,
                                     const std::filesystem::path& png_path,
                                     TileId firstgid, int columns,
                                     int tilewidth, int tileheight,
                                     int tilecount) {
    std::string key =
        std::filesystem::path(png_path).lexically_normal().string();
    textures.try_load(key, png_path.string());
    register_tiles(key, firstgid, columns, tilewidth, tileheight, tilecount);

    std::cout << "[TileCatalog] inline " << name << " firstgid=" << firstgid
              << " tiles=" << tilecount << "\n";
}

SDL_Texture* TileCatalog::texture_for(TileId id) const {
    auto it = tiles.find(id);
    if (it == tiles.end()) return nullptr;
    return textures.get_or_null(it->second.atlas_key);
}

SDL_Rect TileCatalog::src_for(TileId id) const {
    auto it = tiles.find(id);
    if (it == tiles.end()) return SDL_Rect{0, 0, 32, 32};
    return it->second.src;
}

bool TileCatalog::has(TileId id) const { return tiles.count(id) > 0; }
