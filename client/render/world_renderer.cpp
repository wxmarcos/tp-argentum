#include "render/world_renderer.h"

#include <SDL2/SDL.h>

WorldRenderer::WorldRenderer(SDL2pp::Renderer& renderer,
                            const ClientConfig& config):
        renderer(renderer), config(config) {}

void WorldRenderer::render(const ClientGameState& state) {
    using namespace SDL2pp;

    renderer.SetDrawColor(34, 51, 34, 255);
    renderer.Clear();

    const int ts = config.tile_size;
    const int screen_cx = config.window_width / 2;
    const int screen_cy = config.window_height / 2;

    const int cam_offset_x = screen_cx - state.get_player_x() * ts - ts / 2;
    const int cam_offset_y = screen_cy - state.get_player_y() * ts - ts / 2;

    draw_grid(cam_offset_x, cam_offset_y);

    draw_character(state.get_player_x(), state.get_player_y(),
                   state.get_player_dir(), cam_offset_x, cam_offset_y,
                   /*is_self=*/true);

    renderer.Present();
}

void WorldRenderer::draw_grid(int cam_offset_x, int cam_offset_y) {
    using namespace SDL2pp;

    const int ts = config.tile_size;

    renderer.SetDrawColor(50, 70, 50, 255);

    for (int gx = 0; gx <= config.map_width; ++gx) {
        int sx = cam_offset_x + gx * ts;
        renderer.DrawLine(sx, cam_offset_y, sx,
                          cam_offset_y + config.map_height * ts);
    }
    for (int gy = 0; gy <= config.map_height; ++gy) {
        int sy = cam_offset_y + gy * ts;
        renderer.DrawLine(cam_offset_x, sy,
                          cam_offset_x + config.map_width * ts, sy);
    }
}

void WorldRenderer::draw_character(int world_x, int world_y,
                                   protocol::Direction dir, int cam_offset_x,
                                   int cam_offset_y, bool is_self) {
    using namespace SDL2pp;

    const int ts = config.tile_size;
    const int px = cam_offset_x + world_x * ts;
    const int py = cam_offset_y + world_y * ts;

    const int pad = 3;
    Rect body(px + pad, py + pad, ts - 2 * pad, ts - 2 * pad);

    if (is_self) {
        renderer.SetDrawColor(80, 160, 255, 255);
    } else {
        renderer.SetDrawColor(220, 80, 80, 255);
    }
    renderer.FillRect(body);

    renderer.SetDrawColor(255, 255, 255, 255);
    const int cx = px + ts / 2;
    const int cy = py + ts / 2;
    const int len = ts / 2 - pad;

    int dx = 0;
    int dy = 0;
    switch (dir) {
        case protocol::Direction::NORTH:
            dy = -len;
            break;
        case protocol::Direction::SOUTH:
            dy = len;
            break;
        case protocol::Direction::EAST:
            dx = len;
            break;
        case protocol::Direction::WEST:
            dx = -len;
            break;
    }
    renderer.DrawLine(cx, cy, cx + dx, cy + dy);
}
