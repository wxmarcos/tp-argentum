#ifndef CLIENT_RENDER_MAP_DATA_H
#define CLIENT_RENDER_MAP_DATA_H

#include <cstdint>
#include <vector>
#include <string>

#include "render/tile_catalog.h"

// MapData almacena la grilla del mapa con multiples capas.
// Cada celda (x, y, capa) tiene un TileId.
// TileId=0 significa celda vacia (no dibujar).
class MapData {
    private:
    int width;
    int height;
    int layer_count;
    // cells[layer][y * width + x]
    std::vector<std::vector<TileId>> cells;

    // Grilla de colision: true = bloqueado.
    std::vector<bool> collision;

    public:
    MapData(int width, int height, int layer_count);

    void set(int x, int y, int layer, TileId id);
    TileId get(int x, int y, int layer) const;

    void set_collision(int x, int y, bool blocked);
    bool is_blocked(int x, int y) const;

    int get_width()  const { return width; }
    int get_height() const { return height; }
    int get_layers() const { return layer_count; }
};

#endif