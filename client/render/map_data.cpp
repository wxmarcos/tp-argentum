#include "render/map_data.h"

MapData::MapData(int width, int height, int layer_count):
        width(width),
        height(height),
        layer_count(layer_count),
        cells(layer_count,
              std::vector<TileId>(
                  static_cast<size_t>(width * height), 0)),
        collision(static_cast<size_t>(width * height), false) {}

void MapData::set(int x, int y, int layer, TileId id) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    if (layer < 0 || layer >= layer_count) return;
    cells[layer][static_cast<size_t>(y * width + x)] = id;
}

TileId MapData::get(int x, int y, int layer) const {
    if (x < 0 || x >= width || y < 0 || y >= height) return 0;
    if (layer < 0 || layer >= layer_count) return 0;
    return cells[layer][static_cast<size_t>(y * width + x)];
}

void MapData::set_collision(int x, int y, bool blocked) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    collision[static_cast<size_t>(y * width + x)] = blocked;
}

bool MapData::is_blocked(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) return true;
    return collision[static_cast<size_t>(y * width + x)];
}
