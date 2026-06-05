#include "render/tmx_loader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Extrae el valor de un atributo XML.
static std::string attr(const std::string& line,
                        const std::string& name) {
    std::string pat = name + "=\"";
    auto pos = line.find(pat);
    if (pos == std::string::npos) return {};
    pos += pat.size();
    auto end = line.find('"', pos);
    if (end == std::string::npos) return {};
    return line.substr(pos, end - pos);
}

// Parsea una capa CSV y llena el MapData en la capa indicada.
static void parse_csv_layer(const std::string& csv,
                             MapData& map, int layer_idx) {
    std::istringstream ss(csv);
    std::string token;
    int x = 0, y = 0;
    while (std::getline(ss, token, ',')) {
        // Limpiar espacios y saltos de linea
        token.erase(0, token.find_first_not_of(" \t\r\n"));
        token.erase(token.find_last_not_of(" \t\r\n") + 1);
        if (token.empty()) continue;

        uint32_t raw = static_cast<uint32_t>(std::stoul(token));
        uint32_t gid = raw & 0x1FFFFFFFu;

        if (gid != 0) {
            map.set(x, y, layer_idx, static_cast<TileId>(gid));
        }

        x++;
        if (x >= map.get_width()) {
            x = 0;
            y++;
        }
    }
}

// Parsea la capa de colisiones (objectgroup) y marca celdas bloqueadas.
static void parse_collision_layer(std::ifstream& f,
                                   MapData& map, int tile_size) {
    std::string line;
    while (std::getline(f, line)) {
        if (line.find("</objectgroup>") != std::string::npos) break;
        if (line.find("<object") == std::string::npos) continue;

        auto xs = attr(line, "x");
        auto ys = attr(line, "y");
        auto ws = attr(line, "width");
        auto hs = attr(line, "height");
        if (xs.empty() || ys.empty() || ws.empty() || hs.empty())
            continue;

        float px = std::stof(xs);
        float py = std::stof(ys);
        float pw = std::stof(ws);
        float ph = std::stof(hs);

        // Convertir de pixeles a tiles
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

LoadedMap load_tmx(const std::filesystem::path& tmx_path,
                   SDL_Renderer* renderer) {
    std::ifstream f(tmx_path);
    if (!f.is_open()) {
        throw std::runtime_error(
            "[TmxLoader] No se pudo abrir: " + tmx_path.string());
    }

    std::filesystem::path tmx_dir = tmx_path.parent_path();

    // Primera pasada: leer dimensiones del mapa
    int map_w = 0, map_h = 0, tile_size = 32;
    {
        std::ifstream f2(tmx_path);
        std::string line;
        while (std::getline(f2, line)) {
            if (line.find("<map") != std::string::npos) {
                auto w  = attr(line, "width");
                auto h  = attr(line, "height");
                auto tw = attr(line, "tilewidth");
                if (!w.empty())  map_w     = std::stoi(w);
                if (!h.empty())  map_h     = std::stoi(h);
                if (!tw.empty()) tile_size = std::stoi(tw);
                break;
            }
        }
    }

    // Contar capas de tile para saber cuantas necesita MapData
    int layer_count = 0;
    {
        std::ifstream f2(tmx_path);
        std::string line;
        while (std::getline(f2, line)) {
            if (line.find("<layer ") != std::string::npos) {
                layer_count++;
            }
        }
    }

    auto catalog = std::make_unique<TileCatalog>(renderer, tmx_dir);
    auto map     = std::make_unique<MapData>(map_w, map_h,
                                             std::max(1, layer_count));

    std::cout << "[TmxLoader] Mapa " << map_w << "x" << map_h
              << " capas=" << layer_count << "\n";

    // Segunda pasada: parsear tilesets y capas
    f.clear();
    f.seekg(0);

    std::string line;
    int current_layer = 0;
    bool in_data = false;
    std::string csv_buffer;

    while (std::getline(f, line)) {

        // Tileset externo
        if (line.find("<tileset") != std::string::npos &&
            line.find("source=") != std::string::npos) {
            auto src = attr(line, "source");
            auto fg  = attr(line, "firstgid");
            if (!src.empty() && !fg.empty()) {
                std::filesystem::path tsx =
                    (tmx_dir / src).lexically_normal();
                catalog->add_tileset(tsx,
                                     static_cast<TileId>(
                                         std::stoi(fg)));
            }
            continue;
        }

        // Tileset inline (con <image> dentro del TMX)
        if (line.find("<tileset") != std::string::npos &&
            line.find("source=") == std::string::npos) {
            auto fg  = attr(line, "firstgid");
            auto cols= attr(line, "columns");
            auto tw  = attr(line, "tilewidth");
            auto th  = attr(line, "tileheight");
            auto tc  = attr(line, "tilecount");

            std::string img_line;
            while (std::getline(f, img_line)) {
                if (img_line.find("<image") != std::string::npos)
                    break;
                if (img_line.find("</tileset>") != std::string::npos)
                    break;
            }
            auto src = attr(img_line, "source");
            if (!src.empty() && !fg.empty()) {
                std::filesystem::path png =
                    (tmx_dir / src).lexically_normal();
                catalog->add_inline_tileset(
                    png.stem().string(), png,
                    static_cast<TileId>(std::stoi(fg)),
                    cols.empty() ? 1  : std::stoi(cols),
                    tw.empty()   ? 32 : std::stoi(tw),
                    th.empty()   ? 32 : std::stoi(th),
                    tc.empty()   ? 1  : std::stoi(tc));
            }
            continue;
        }

        // Inicio de capa de tile
        if (line.find("<layer ") != std::string::npos) {
            continue;
        }

        // Inicio de datos CSV
        if (line.find("<data encoding=\"csv\"") != std::string::npos) {
            in_data = true;
            csv_buffer.clear();
            continue;
        }

        // Fin de datos CSV
        if (in_data && line.find("</data>") != std::string::npos) {
            in_data = false;
            parse_csv_layer(csv_buffer, *map, current_layer);
            current_layer++;
            continue;
        }

        // Acumular datos CSV
        if (in_data) {
            csv_buffer += line + "\n";
            continue;
        }

        // Capa de colisiones
        if (line.find("<objectgroup") != std::string::npos &&
            line.find("Colision") != std::string::npos) {
            parse_collision_layer(f, *map, tile_size);
            continue;
        }
    }

    std::cout << "[TmxLoader] Cargado: " << tmx_path.filename()
              << " (" << current_layer << " capas)\n";

    return LoadedMap{std::move(catalog), std::move(map)};
}
