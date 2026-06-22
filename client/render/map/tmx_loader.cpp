#include "render/map/tmx_loader.h"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

std::string TmxLoader::attr(const std::string& line,
                            const std::string& name) const {
    std::string pat = name + "=\"";
    auto pos = line.find(pat);
    if (pos == std::string::npos) return {};
    pos += pat.size();
    auto end = line.find('"', pos);
    if (end == std::string::npos) return {};
    return line.substr(pos, end - pos);
}

void TmxLoader::read_map_header(const std::filesystem::path& tmx_path,
                                int& map_w, int& map_h, int& tile_size) const {
    std::ifstream f(tmx_path);
    std::string line;
    while (std::getline(f, line)) {
        if (line.find("<map") != std::string::npos) {
            auto w = attr(line, "width");
            auto h = attr(line, "height");
            auto tw = attr(line, "tilewidth");
            if (!w.empty()) map_w = std::stoi(w);
            if (!h.empty()) map_h = std::stoi(h);
            if (!tw.empty()) tile_size = std::stoi(tw);
            break;
        }
    }
}

int TmxLoader::count_layers(const std::filesystem::path& tmx_path) const {
    std::ifstream f(tmx_path);
    std::string line;
    int layer_count = 0;
    while (std::getline(f, line)) {
        if (line.find("<layer ") != std::string::npos) {
            layer_count++;
        }
    }
    return layer_count;
}

void TmxLoader::parse_csv_layer(const std::string& csv, MapData& map,
                                int layer_idx) const {
    std::istringstream ss(csv);
    std::string token;
    int x = 0, y = 0;
    while (std::getline(ss, token, ',')) {
        token.erase(0, token.find_first_not_of(" \t\r\n"));
        token.erase(token.find_last_not_of(" \t\r\n") + 1);
        if (token.empty()) continue;

        uint32_t raw = static_cast<uint32_t>(std::stoul(token));
        uint32_t gid = raw & 0x1FFFFFFFu;

        if (gid != 0) {
            map.set(x, y, layer_idx, static_cast<TileId>(gid));
            uint8_t flip = 0;
            if (raw & 0x80000000u) flip |= 0x4;
            if (raw & 0x40000000u) flip |= 0x2;
            if (raw & 0x20000000u) flip |= 0x1;
            map.set_flip(x, y, layer_idx, flip);
        }

        x++;
        if (x >= map.get_width()) {
            x = 0;
            y++;
        }
    }
}

void TmxLoader::parse_collision_layer(std::ifstream& f, MapData& map,
                                      int tile_size) const {
    std::string line;
    while (std::getline(f, line)) {
        if (line.find("</objectgroup>") != std::string::npos) break;
        if (line.find("<object") == std::string::npos) continue;

        auto xs = attr(line, "x");
        auto ys = attr(line, "y");
        auto ws = attr(line, "width");
        auto hs = attr(line, "height");
        if (xs.empty() || ys.empty() || ws.empty() || hs.empty()) continue;

        float px = std::stof(xs);
        float py = std::stof(ys);
        float pw = std::stof(ws);
        float ph = std::stof(hs);

        int tx1 = static_cast<int>(px) / tile_size;
        int ty1 = static_cast<int>(py) / tile_size;
        int tx2 = static_cast<int>(px + pw - 1) / tile_size;
        int ty2 = static_cast<int>(py + ph - 1) / tile_size;

        for (int ty = ty1; ty <= ty2; ++ty) {
            for (int tx = tx1; tx <= tx2; ++tx) {
                map.set_collision(tx, ty, true);
            }
        }
    }
}

void TmxLoader::parse_external_tileset(const std::string& line,
                                       const std::filesystem::path& tmx_dir,
                                       TileCatalog& catalog) {
    auto src = attr(line, "source");
    auto fg = attr(line, "firstgid");
    if (src.empty() || fg.empty()) return;
    std::filesystem::path tsx = (tmx_dir / src).lexically_normal();
    catalog.add_tileset(tsx, static_cast<TileId>(std::stoi(fg)));
}

void TmxLoader::parse_inline_tileset(std::ifstream& f, const std::string& line,
                                     const std::filesystem::path& tmx_dir,
                                     TileCatalog& catalog) {
    auto fg = attr(line, "firstgid");
    auto cols = attr(line, "columns");
    auto tw = attr(line, "tilewidth");
    auto th = attr(line, "tileheight");
    auto tc = attr(line, "tilecount");

    std::string img_line;
    while (std::getline(f, img_line)) {
        if (img_line.find("<image") != std::string::npos) break;
        if (img_line.find("</tileset>") != std::string::npos) break;
    }
    auto src = attr(img_line, "source");
    if (src.empty() || fg.empty()) return;

    std::filesystem::path png = (tmx_dir / src).lexically_normal();
    catalog.add_inline_tileset(
        png, static_cast<TileId>(std::stoi(fg)),
        cols.empty() ? 1 : std::stoi(cols), tw.empty() ? 32 : std::stoi(tw),
        th.empty() ? 32 : std::stoi(th), tc.empty() ? 1 : std::stoi(tc));
}

void TmxLoader::parse_body(std::ifstream& f,
                           const std::filesystem::path& tmx_dir,
                           TileCatalog& catalog, MapData& map, int tile_size) {
    f.clear();
    f.seekg(0);

    std::string line;
    int current_layer = 0;
    bool in_data = false;
    std::string csv_buffer;

    while (std::getline(f, line)) {
        const bool is_tileset = line.find("<tileset") != std::string::npos;
        if (is_tileset && line.find("source=") != std::string::npos) {
            parse_external_tileset(line, tmx_dir, catalog);
            continue;
        }
        if (is_tileset) {
            parse_inline_tileset(f, line, tmx_dir, catalog);
            continue;
        }
        if (line.find("<layer ") != std::string::npos) {
            continue;
        }
        if (line.find("<data encoding=\"csv\"") != std::string::npos) {
            in_data = true;
            csv_buffer.clear();
            continue;
        }
        if (in_data && line.find("</data>") != std::string::npos) {
            in_data = false;
            parse_csv_layer(csv_buffer, map, current_layer);
            current_layer++;
            continue;
        }
        if (in_data) {
            csv_buffer += line + "\n";
            continue;
        }
        if (line.find("<objectgroup") != std::string::npos &&
            line.find("Colision") != std::string::npos) {
            parse_collision_layer(f, map, tile_size);
            continue;
        }
    }
}

LoadedMap TmxLoader::load(const std::filesystem::path& tmx_path,
                          SDL_Renderer* renderer) {
    std::ifstream f(tmx_path);
    if (!f.is_open()) {
        throw std::runtime_error("[TmxLoader] No se pudo abrir: " +
                                 tmx_path.string());
    }

    const std::filesystem::path tmx_dir = tmx_path.parent_path();

    int map_w = 0;
    int map_h = 0;
    int tile_size = 32;
    read_map_header(tmx_path, map_w, map_h, tile_size);
    const int layer_count = count_layers(tmx_path);

    auto catalog = std::make_unique<TileCatalog>(renderer, tmx_dir);
    auto map =
        std::make_unique<MapData>(map_w, map_h, std::max(1, layer_count));

    std::cout << "[TmxLoader] Mapa " << map_w << "x" << map_h
              << " capas=" << layer_count << "\n";

    parse_body(f, tmx_dir, *catalog, *map, tile_size);

    std::cout << "[TmxLoader] Cargado: " << tmx_path.filename() << "\n";

    return LoadedMap{std::move(catalog), std::move(map)};
}
