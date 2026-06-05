#include "render/world_renderer.h"

#include <SDL2/SDL.h>

#include <algorithm>
#include <filesystem>

static constexpr int ANIM_FRAMES = 4;
static constexpr int ANIM_MS_FRAME = 150;

namespace {
int body_scale_pct(const std::string& raza) {
    if (raza == "enano") return 85;
    if (raza == "gnomo") return 82;
    return 100;
}
int head_scale_pct(const std::string& raza) {
    if (raza == "gnomo") return 85;
    return 100;
}

int creature_scale_pct(const std::string& type) {
    if (type == "golem") return 170;
    if (type == "orco") return 150;
    if (type == "arana") return 130;
    if (type == "esqueleto") return 110;
    if (type == "zombie") return 105;
    if (type == "goblin") return 90;
    return 100;
}
}  // namespace

WorldRenderer::WorldRenderer(SDL2pp::Renderer& renderer,
                             const ClientConfig& config):
    renderer(renderer),
    config(config), registry(renderer.Get(), std::filesystem::current_path() /
                                                 config.assets_path),
    local_anim(ANIM_FRAMES, ANIM_MS_FRAME) {}

int WorldRenderer::dir_to_idx(protocol::Direction dir) {
    switch (dir) {
        case protocol::Direction::SOUTH:
            return DIR_SOUTH;
        case protocol::Direction::NORTH:
            return DIR_NORTH;
        case protocol::Direction::EAST:
            return DIR_EAST;
        case protocol::Direction::WEST:
            return DIR_WEST;
    }
    return DIR_SOUTH;
}

void WorldRenderer::visible_tile_range(int cam_offset_x, int cam_offset_y,
                                       int& first_gx, int& last_gx,
                                       int& first_gy, int& last_gy) const {
    const int ts = config.tile_size;

    auto floor_div = [](int a, int b) {
        return a / b - (a % b != 0 && (a ^ b) < 0);
    };

    first_gx = std::max(0, floor_div(-cam_offset_x, ts));
    first_gy = std::max(0, floor_div(-cam_offset_y, ts));
    last_gx = std::min(config.map_width - 1,
                       floor_div(config.window_width - cam_offset_x, ts));
    last_gy = std::min(config.map_height - 1,
                       floor_div(config.window_height - cam_offset_y, ts));
}

void WorldRenderer::draw_tiles(int cam_offset_x, int cam_offset_y) {
    const int ts = config.tile_size;

    int first_gx, last_gx, first_gy, last_gy;
    visible_tile_range(cam_offset_x, cam_offset_y, first_gx, last_gx, first_gy,
                       last_gy);

    SDL_Texture* tile_tex = registry.get_texture("tile_grass");

    for (int gy = first_gy; gy <= last_gy; ++gy) {
        for (int gx = first_gx; gx <= last_gx; ++gx) {
            const int px = cam_offset_x + gx * ts;
            const int py = cam_offset_y + gy * ts;
            SDL_Rect dst{px, py, ts, ts};

            if (tile_tex) {
                SDL_Rect src = TILE_GRASS_SRC;
                SDL_RenderCopy(renderer.Get(), tile_tex, &src, &dst);
            } else {
                if ((gx + gy) % 2 == 0) {
                    renderer.SetDrawColor(40, 58, 40, 255);
                } else {
                    renderer.SetDrawColor(30, 46, 30, 255);
                }
                renderer.FillRect(SDL2pp::Rect(px, py, ts, ts));
            }
        }
    }
}

void WorldRenderer::draw_character(int world_x, int world_y,
                                   protocol::Direction dir,
                                   const std::string& sprite_key,
                                   const std::string& raza, int frame,
                                   int cam_offset_x, int cam_offset_y) {
    const int ts = config.tile_size;
    const int px = cam_offset_x + world_x * ts;
    const int py = cam_offset_y + world_y * ts;

    if (px + ts * 2 < 0 || px > config.window_width || py + ts * 2 < 0 ||
        py > config.window_height) {
        return;
    }

    const int dir_idx = dir_to_idx(dir);

    const int body_scale = body_scale_pct(raza);
    const int head_scale = head_scale_pct(raza);

    const int body_h = (ts * 2 * body_scale) / 100;
    const int body_top = py + ts - body_h;

    // cuerpo
    SDL_Texture* body_tex = registry.get_texture(sprite_key);
    if (body_tex) {
        SDL_Rect src = registry.get_frame(sprite_key, dir_idx, frame);
        const int draw_w = src.h > 0 ? (src.w * body_h) / src.h : ts;

        SDL_Rect dst{px + (ts - draw_w) / 2, body_top, draw_w, body_h};

        SDL_RenderCopy(renderer.Get(), body_tex, &src, &dst);

    } else {
        const int pad = 3;
        renderer.SetDrawColor(80, 160, 255, 255);
        renderer.FillRect(
            SDL2pp::Rect(px + pad, py + pad, ts - 2 * pad, ts - 2 * pad));
    }

    // cabeza (encima del cuerpo, apoyada sobre los hombros)
    SDL_Texture* head_tex = registry.get_head_texture(raza);
    if (head_tex) {
        SDL_Rect src = registry.get_head_rect(raza, dir_idx);

        const int head_h =
            (((ts * 7) / 8) * body_scale / 100) * head_scale / 100;
        const int head_w = src.h > 0 ? (src.w * head_h) / src.h : ts / 2;

        const int neck = registry.get_head_neck(sprite_key) * body_scale / 100;
        const int head_bottom = body_top + neck;

        SDL_Rect dst{px + (ts - head_w) / 2, head_bottom - head_h, head_w,
                     head_h};

        SDL_RenderCopy(renderer.Get(), head_tex, &src, &dst);
    }
}

void WorldRenderer::draw_creature(int world_x, int world_y,
                                  protocol::Direction dir,
                                  const std::string& type, int frame,
                                  int cam_offset_x, int cam_offset_y) {
    const int ts = config.tile_size;
    const int px = cam_offset_x + world_x * ts;
    const int py = cam_offset_y + world_y * ts;

    const int scale = creature_scale_pct(type);
    const int body_h = (ts * 2 * scale) / 100;

    if (px + body_h < 0 || px - body_h > config.window_width || py + ts < 0 ||
        py - body_h > config.window_height) {
        return;
    }

    const int dir_idx = dir_to_idx(dir);
    const int body_top = py + ts - body_h;

    SDL_Texture* tex = registry.get_texture(type);
    if (!tex) {
        return;
    }
    SDL_Rect src = registry.get_frame(type, dir_idx, frame);
    const int draw_w = src.h > 0 ? (src.w * body_h) / src.h : ts;
    SDL_Rect dst{px + (ts - draw_w) / 2, body_top, draw_w, body_h};
    SDL_RenderCopy(renderer.Get(), tex, &src, &dst);
}

void WorldRenderer::render(const ClientGameState& state, uint32_t delta_ms) {
    renderer.SetDrawColor(34, 51, 34, 255);
    renderer.Clear();

    const int ts = config.tile_size;
    const int screen_cx = config.window_width / 2;
    const int screen_cy = config.window_height / 2;

    int cam_x, cam_y;
    if (state.has_local_position()) {
        cam_x = state.get_local_x();
        cam_y = state.get_local_y();
    } else {
        cam_x = state.get_map_width() / 2;
        cam_y = state.get_map_height() / 2;
    }

    const int cam_offset_x = screen_cx - cam_x * ts - ts / 2;
    const int cam_offset_y = screen_cy - cam_y * ts - ts / 2;

    draw_tiles(cam_offset_x, cam_offset_y);

    // jugador local
    if (state.has_local_position()) {
        local_anim.update(delta_ms, state.get_local_dir(),
                          state.get_local_moved());

        draw_character(state.get_local_x(), state.get_local_y(),
                       state.get_local_dir(), config.character_clase,
                       config.character_raza, local_anim.current_frame(),
                       cam_offset_x, cam_offset_y);
    }

    // otros jugadores
    for (const auto& [nick, pv] : state.get_others()) {
        auto [it, inserted] =
            other_anims.try_emplace(nick, ANIM_FRAMES, ANIM_MS_FRAME);

        it->second.update(delta_ms, pv.direction, pv.moved);

        draw_character(pv.x, pv.y, pv.direction, "humano", "humano",
                       it->second.current_frame(), cam_offset_x, cam_offset_y);
    }

    for (auto it = other_anims.begin(); it != other_anims.end();) {
        if (!state.get_others().count(it->first)) {
            it = other_anims.erase(it);
        } else {
            ++it;
        }
    }

    // criaturas/npcs
    for (const auto& [key, cv] : state.get_creatures()) {
        auto [it, inserted] =
            creature_anims.try_emplace(key, ANIM_FRAMES, ANIM_MS_FRAME);

        it->second.update(delta_ms, cv.direction, cv.moved);

        draw_creature(cv.x, cv.y, cv.direction, cv.type,
                      it->second.current_frame(), cam_offset_x, cam_offset_y);
    }

    for (auto cit = creature_anims.begin(); cit != creature_anims.end();) {
        if (!state.get_creatures().count(cit->first)) {
            cit = creature_anims.erase(cit);
        } else {
            ++cit;
        }
    }

    renderer.Present();
}
