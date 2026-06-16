#include "render/world_renderer.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "game/entity_keys.h"
#include "render/colors.h"

static constexpr int ANIM_FRAMES = 4;
static constexpr int ANIM_MS_FRAME = 150;
static constexpr Uint32 DEATH_ANIM_MS = 900;

static constexpr int CHARACTER_HEIGHT_TILES = 2;
static constexpr int PLACEHOLDER_PAD = 3;
static constexpr uint32_t FLOATING_TEXT_LIFETIME_MS = 1000;
static constexpr float FLOATING_TEXT_RISE_PX = 24.0f;
static constexpr float EFFECT_HEIGHT_TILES = 2.6f;

static constexpr SDL_Color PLACEHOLDER_BODY{80, 160, 255, 255};
static constexpr SDL_Color CHECKER_LIGHT{40, 58, 40, 255};
static constexpr SDL_Color CHECKER_DARK{30, 46, 30, 255};

static constexpr int WEAPON_REF_H = 48;  // alto del personaje en el sheet del arma
static constexpr int WEAPON_OFF_X[4] = {8, -8, 10, -10};   // S, N, E, W
static constexpr int WEAPON_OFF_Y[4] = {20, 20, 20, 20};

WorldRenderer::WorldRenderer(SDL2pp::Renderer& renderer,
                             const ClientConfig& config):
        renderer(renderer),
        config(config),
        registry(renderer.Get(),
                 std::filesystem::current_path() / config.assets_path),
        text(renderer.Get(),
             (std::filesystem::current_path() / config.font_path)
                 .lexically_normal(),
             config.font_size),
        item_sprites(renderer, config),
        weapon_sprites(renderer, config),
        local_anim(ANIM_FRAMES, ANIM_MS_FRAME) {
    load_map(config.map_name);
    load_effects();
}

void WorldRenderer::load_map(const std::string& map_name) {
    std::filesystem::path tmx_path = std::filesystem::current_path() /
                                     config.assets_path / ".." / "mapa" /
                                     (map_name + ".tmx");
    tmx_path = tmx_path.lexically_normal();

    try {
        auto loaded = TmxLoader().load(tmx_path, renderer.Get());
        catalog = std::move(loaded.catalog);
        map = std::move(loaded.map);
        std::cout << "[WorldRenderer] Mapa cargado: " << map_name << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[WorldRenderer] Error cargando mapa id=" << map_name
                  << ": " << e.what() << "\n";
    }
}

void WorldRenderer::load_map_by_id(uint16_t map_id) {
    load_map(config.map_name_for(map_id));
    loaded_map_id = map_id;
}

void WorldRenderer::update_loaded_map(const ClientGameState& state) {
    const uint16_t current_map_id = state.get_current_map_id();
    if (current_map_id != 0 && current_map_id != loaded_map_id) {
        load_map_by_id(current_map_id);
    }
}

int WorldRenderer::dir_to_idx(protocol::Direction dir) {
    switch (dir) {
        case protocol::Direction::SOUTH: return DIR_SOUTH;
        case protocol::Direction::NORTH: return DIR_NORTH;
        case protocol::Direction::EAST: return DIR_EAST;
        case protocol::Direction::WEST: return DIR_WEST;
    }
    return DIR_SOUTH;
}

int WorldRenderer::body_scale_pct(const std::string& raza) const {
    if (raza == keys::ENANO) return 85;
    if (raza == keys::GNOMO) return 82;
    return 100;
}

int WorldRenderer::head_scale_pct(const std::string& raza) const {
    if (raza == keys::GNOMO) return 85;
    return 100;
}

int WorldRenderer::creature_scale_pct(const std::string& type) const {
    if (type == keys::GOLEM) return 170;
    if (type == keys::ORCO) return 150;
    if (type == keys::ARANA) return 130;
    if (type == keys::ESQUELETO) return 110;
    if (type == keys::ZOMBIE) return 105;
    if (type == keys::GOBLIN) return 90;
    return 100;
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
                       floor_div(config.game_area_width() - cam_offset_x, ts));
    last_gy = std::min(config.map_height - 1,
                       floor_div(config.window_height - cam_offset_y, ts));
}

void WorldRenderer::compute_camera(const ClientGameState& state,
                                   int& cam_offset_x, int& cam_offset_y) const {
    const int ts = config.tile_size;
    const int screen_cx = config.game_area_width() / 2;
    const int screen_cy = config.window_height / 2;

    int cam_x;
    int cam_y;
    if (state.has_local_position()) {
        cam_x = state.get_local_x();
        cam_y = state.get_local_y();
    } else {
        cam_x = state.get_map_width() / 2;
        cam_y = state.get_map_height() / 2;
    }

    cam_offset_x = screen_cx - cam_x * ts - ts / 2;
    cam_offset_y = screen_cy - cam_y * ts - ts / 2;
}

void WorldRenderer::draw_layers(int cam_offset_x, int cam_offset_y) {
    const int layers = map ? map->get_layers() : 1;
    for (int layer = 0; layer < layers; ++layer) {
        draw_map_layer(layer, cam_offset_x, cam_offset_y);
    }
}

void WorldRenderer::draw_fallback_layer(int cam_offset_x, int cam_offset_y) {
    const int ts = config.tile_size;
    int first_gx, last_gx, first_gy, last_gy;
    visible_tile_range(cam_offset_x, cam_offset_y, first_gx, last_gx, first_gy,
                       last_gy);
    for (int gy = first_gy; gy <= last_gy; ++gy) {
        for (int gx = first_gx; gx <= last_gx; ++gx) {
            const SDL_Color c =
                ((gx + gy) % 2 == 0) ? CHECKER_LIGHT : CHECKER_DARK;
            renderer.SetDrawColor(c.r, c.g, c.b, c.a);
            renderer.FillRect(SDL2pp::Rect(cam_offset_x + gx * ts,
                                           cam_offset_y + gy * ts, ts, ts));
        }
    }
}

void WorldRenderer::blit_tile(SDL_Texture* tex, const SDL_Rect& src,
                              const SDL_Rect& dst, uint8_t flip) {
    if (flip == 0) {
        SDL_RenderCopy(renderer.Get(), tex, &src, &dst);
        return;
    }
    double angle = 0.0;
    SDL_RendererFlip sdl_flip = SDL_FLIP_NONE;
    const bool d = flip & 0x1;
    const bool h = flip & 0x4;
    const bool v = flip & 0x2;

    if (d) {
        if (h && v) {
            angle = 270.0;
            sdl_flip = SDL_FLIP_HORIZONTAL;
        } else if (h) {
            angle = 90.0;
        } else if (v) {
            angle = 270.0;
        } else {
            angle = 90.0;
            sdl_flip = SDL_FLIP_HORIZONTAL;
        }
    } else {
        if (h)
            sdl_flip =
                static_cast<SDL_RendererFlip>(sdl_flip | SDL_FLIP_HORIZONTAL);
        if (v)
            sdl_flip =
                static_cast<SDL_RendererFlip>(sdl_flip | SDL_FLIP_VERTICAL);
    }
    SDL_RenderCopyEx(renderer.Get(), tex, &src, &dst, angle, nullptr,
                     sdl_flip);
}

void WorldRenderer::draw_map_layer(int layer, int cam_offset_x,
                                   int cam_offset_y) {
    if (!map || !catalog) {
        draw_fallback_layer(cam_offset_x, cam_offset_y);
        return;
    }

    const int ts = config.tile_size;
    int first_gx, last_gx, first_gy, last_gy;
    visible_tile_range(cam_offset_x, cam_offset_y, first_gx, last_gx, first_gy,
                       last_gy);

    for (int gy = first_gy; gy <= last_gy; ++gy) {
        for (int gx = first_gx; gx <= last_gx; ++gx) {
            TileId tid = map->get(gx, gy, layer);
            if (tid == 0) continue;
            SDL_Texture* tex = catalog->texture_for(tid);
            if (!tex) continue;

            SDL_Rect src = catalog->src_for(tid);
            SDL_Rect dst{cam_offset_x + gx * ts, cam_offset_y + gy * ts, src.w,
                         src.h};
            blit_tile(tex, src, dst, map->get_flip(gx, gy, layer));
        }
    }
}

void WorldRenderer::draw_body(const std::string& sprite_key, int dir_idx,
                              int frame, int px, int py, int body_top,
                              int body_h) {
    const int ts = config.tile_size;
    SDL_Texture* body_tex = registry.get_texture(sprite_key);
    if (!body_tex) {
        const int pad = PLACEHOLDER_PAD;
        renderer.SetDrawColor(PLACEHOLDER_BODY.r, PLACEHOLDER_BODY.g,
                              PLACEHOLDER_BODY.b, PLACEHOLDER_BODY.a);
        renderer.FillRect(
            SDL2pp::Rect(px + pad, py + pad, ts - 2 * pad, ts - 2 * pad));
        return;
    }
    SDL_Rect src = registry.get_frame(sprite_key, dir_idx, frame);
    const int draw_w = src.h > 0 ? (src.w * body_h) / src.h : ts;
    SDL_Rect dst{px + (ts - draw_w) / 2, body_top, draw_w, body_h};
    SDL_RenderCopy(renderer.Get(), body_tex, &src, &dst);
}

void WorldRenderer::draw_head(const std::string& sprite_key,
                              const std::string& raza, int dir_idx, int px,
                              int body_top, int body_scale) {
    SDL_Texture* head_tex = registry.get_head_texture(raza);
    if (!head_tex) {
        return;
    }
    const int ts = config.tile_size;
    const int head_scale = head_scale_pct(raza);
    SDL_Rect src = registry.get_head_rect(raza, dir_idx);

    const int head_h = (((ts * 7) / 8) * body_scale / 100) * head_scale / 100;
    const int head_w = src.h > 0 ? (src.w * head_h) / src.h : ts / 2;

    const int neck = registry.get_head_neck(sprite_key) * body_scale / 100;
    const int head_bottom = body_top + neck;

    SDL_Rect dst{px + (ts - head_w) / 2, head_bottom - head_h, head_w, head_h};
    SDL_RenderCopy(renderer.Get(), head_tex, &src, &dst);
}

void WorldRenderer::draw_character(int world_x, int world_y,
                                   protocol::Direction dir,
                                   const std::string& sprite_key,
                                   const std::string& raza, int frame,
                                   int cam_offset_x, int cam_offset_y,
                                   const std::string& weapon_name) {
    const int ts = config.tile_size;
    const int px = cam_offset_x + world_x * ts;
    const int py = cam_offset_y + world_y * ts;

    if (px + ts * CHARACTER_HEIGHT_TILES < 0 || px > config.game_area_width() ||
        py + ts * CHARACTER_HEIGHT_TILES < 0 || py > config.window_height) {
        return;
    }

    const int dir_idx = dir_to_idx(dir);
    const int body_scale = body_scale_pct(raza);
    const int body_h = (ts * CHARACTER_HEIGHT_TILES * body_scale) / 100;
    const int body_top = py + ts - body_h;

    draw_body(sprite_key, dir_idx, frame, px, py, body_top, body_h);
    draw_head(sprite_key, raza, dir_idx, px, body_top, body_scale);

    if (!weapon_name.empty()) {
        draw_weapon(weapon_name, dir_idx, px, body_top, body_h);
    }
}

void WorldRenderer::draw_ghost(int world_x, int world_y,
                               protocol::Direction dir, int frame,
                               int cam_offset_x, int cam_offset_y) {
    const int ts = config.tile_size;
    const int px = cam_offset_x + world_x * ts;
    const int py = cam_offset_y + world_y * ts;
    if (px + ts * CHARACTER_HEIGHT_TILES < 0 || px > config.window_width ||
        py + ts * CHARACTER_HEIGHT_TILES < 0 || py > config.window_height) {
        return;
    }
    const std::string ghost_key(keys::FANTASMA);
    SDL_Texture* tex = registry.get_texture(ghost_key);
    if (!tex) return;

    const SDL_Rect src = registry.get_frame(ghost_key, dir_to_idx(dir), frame);
    const int body_h = ts * CHARACTER_HEIGHT_TILES;
    const int body_top = py + ts - body_h;
    const int draw_w = src.h > 0 ? (src.w * body_h) / src.h : ts;
    const SDL_Rect dst{px + (ts - draw_w) / 2, body_top, draw_w, body_h};
    SDL_RenderCopy(renderer.Get(), tex, &src, &dst);
}

void WorldRenderer::draw_player(const std::string& nick, bool dead,
                                int world_x, int world_y,
                                protocol::Direction dir,
                                const std::string& sprite_key,
                                const std::string& raza, int frame,
                                int cam_offset_x, int cam_offset_y,
                                const std::string& weapon_name) {
    if (dead) {
        const Uint32 now = SDL_GetTicks();
        auto [it, ins] = ghost_since.try_emplace(nick, now);
        if (now - it->second >= DEATH_ANIM_MS) {
            draw_ghost(world_x, world_y, dir, frame, cam_offset_x,
                       cam_offset_y);
            return;
        }
    } else {
        ghost_since.erase(nick);
    }
    draw_character(world_x, world_y, dir, sprite_key, raza, frame, cam_offset_x,
                   cam_offset_y, weapon_name);
}

void WorldRenderer::draw_creature(int world_x, int world_y,
                                  protocol::Direction dir,
                                  const std::string& type, int frame,
                                  int cam_offset_x, int cam_offset_y) {
    const int ts = config.tile_size;
    const int px = cam_offset_x + world_x * ts;
    const int py = cam_offset_y + world_y * ts;

    const int scale = creature_scale_pct(type);
    const int body_h = (ts * CHARACTER_HEIGHT_TILES * scale) / 100;

    if (px + body_h < 0 || px - body_h > config.game_area_width() ||
        py + ts < 0 || py - body_h > config.window_height) {
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

void WorldRenderer::draw_name(const std::string& nick, int world_x,
                              int world_y, int cam_offset_x, int cam_offset_y) {
    if (!text.ok() || nick.empty()) {
        return;
    }
    const int ts = config.tile_size;
    const int center_x = cam_offset_x + world_x * ts + ts / 2;
    const int top_y = cam_offset_y + world_y * ts - ts - text.line_height();

    text.draw_centered(nick, center_x + 1, top_y + 1, colors::BLACK);
    text.draw_centered(nick, center_x, top_y, colors::WHITE);
}

void WorldRenderer::draw_floor_items(const ClientGameState& state,
                                     int cam_offset_x, int cam_offset_y) {
    const int ts = config.tile_size;
    for (const auto& entry : state.get_floor_items()) {
        const FloorItem& item = entry.second;
        const int dx = cam_offset_x + item.x * ts;
        const int dy = cam_offset_y + item.y * ts;
        const SDL_Rect dst{dx, dy, ts, ts};
        if (const ItemSprite* s = item_sprites.find(item.name)) {
            SDL_RenderCopy(renderer.Get(), s->tex, &s->src, &dst);
        } else {
            renderer.SetDrawColor(190, 160, 70, 255);
            renderer.FillRect(
                SDL2pp::Rect(dx + ts / 4, dy + ts / 4, ts / 2, ts / 2));
        }
    }
}

std::string WorldRenderer::local_weapon_name(
    const ClientGameState& state) const {
    for (const auto& slot : state.get_inventory()) {
        if (slot.equipado && weapon_sprites.find(slot.item)) {
            return slot.item;
        }
    }
    return "";
}

void WorldRenderer::draw_weapon(const std::string& weapon_name, int dir_idx,
                                int px, int body_top, int body_h) {
    if (dir_idx == DIR_NORTH) {
        return;
    }
    const WeaponSprite* w = weapon_sprites.find(weapon_name);
    if (!w || !w->tex) {
        return;
    }
    const SDL_Rect& src = w->rects[dir_idx];
    const int draw_h = src.h * body_h / WEAPON_REF_H;
    const int draw_w = src.w * body_h / WEAPON_REF_H;
    const int cx = px + config.tile_size / 2;
    const SDL_Rect dst{cx + WEAPON_OFF_X[dir_idx] - draw_w / 2,
                       body_top + WEAPON_OFF_Y[dir_idx], draw_w, draw_h};
    SDL_RenderCopy(renderer.Get(), w->tex, &src, &dst);
}

void WorldRenderer::draw_meditation_effect(int world_x, int world_y,
                                           int cam_offset_x, int cam_offset_y) {
    const EffectData& d = effect_data_map[EffectKind::Meditar];
    SDL_Texture* tex = effect_tex[EffectKind::Meditar];
    if (!tex || d.frames.empty() || d.ms_per_frame <= 0) {
        return;
    }

    const int n = static_cast<int>(d.frames.size());
    const int fi = static_cast<int>(SDL_GetTicks() / d.ms_per_frame) % n;
    const SDL_Rect& src = d.frames[fi];
    if (src.h <= 0) {
        return;
    }

    const int ts = config.tile_size;
    const float target_h = ts * EFFECT_HEIGHT_TILES;
    const float scale = target_h / src.h;
    const int dw = static_cast<int>(src.w * scale);
    const int dh = static_cast<int>(target_h);
    const int cx = cam_offset_x + world_x * ts + ts / 2;
    const int cy = cam_offset_y + world_y * ts + ts / 2;
    const SDL_Rect dst{cx - dw / 2, cy - dh / 2, dw, dh};
    SDL_RenderCopy(renderer.Get(), tex, &src, &dst);
}

void WorldRenderer::draw_local(const ClientGameState& state, uint32_t delta_ms,
                               int cam_offset_x, int cam_offset_y) {
    if (!state.has_local_position()) {
        return;
    }
    local_anim.update(delta_ms, state.get_local_dir(),
                      state.get_local_moved());

    std::string local_clase = config.character_clase;
    std::string local_raza = config.character_raza;
    if (state.has_local_stats()) {
        const PlayerStats& s = state.get_local_stats();
        if (!s.clase.empty()) local_clase = s.clase;
        if (!s.raza.empty()) local_raza = s.raza;
    }

    const std::string& nick = state.get_local_nick();
    draw_player(nick, state.is_dead(nick), state.get_local_x(),
                state.get_local_y(), state.get_local_dir(),
                local_body_key(state, local_clase), local_raza,
                local_anim.current_frame(), cam_offset_x, cam_offset_y,
                local_weapon_name(state));

    if (state.is_meditating(nick)) {
        draw_meditation_effect(state.get_local_x(), state.get_local_y(),
                               cam_offset_x, cam_offset_y);
    }

    draw_name(nick, state.get_local_x(), state.get_local_y(), cam_offset_x,
              cam_offset_y);
}

void WorldRenderer::draw_others(const ClientGameState& state,
                                uint32_t delta_ms, int cam_offset_x,
                                int cam_offset_y) {
    for (const auto& [nick, pv] : state.get_others()) {
        auto [it, inserted] =
            other_anims.try_emplace(nick, ANIM_FRAMES, ANIM_MS_FRAME);
        it->second.update(delta_ms, pv.direction, pv.moved);

        draw_player(nick, state.is_dead(nick), pv.x, pv.y, pv.direction,
                    pv.clase.empty() ? std::string(keys::HUMANO) : pv.clase,
                    pv.raza.empty() ? std::string(keys::HUMANO) : pv.raza,
                    it->second.current_frame(), cam_offset_x, cam_offset_y);

        if (state.is_meditating(nick)) {
            draw_meditation_effect(pv.x, pv.y, cam_offset_x, cam_offset_y);
        }

        draw_name(pv.nick, pv.x, pv.y, cam_offset_x, cam_offset_y);
    }

    for (auto it = other_anims.begin(); it != other_anims.end();) {
        if (!state.get_others().count(it->first)) {
            it = other_anims.erase(it);
        } else {
            ++it;
        }
    }
}

void WorldRenderer::draw_all_creatures(const ClientGameState& state,
                                       uint32_t delta_ms, int cam_offset_x,
                                       int cam_offset_y) {
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
}

void WorldRenderer::draw_floating_texts(const ClientGameState& state,
                                        uint32_t delta_ms, int cam_offset_x,
                                        int cam_offset_y) {
    for (const auto& ev : state.get_floating_events()) {
        SDL_Color color;
        switch (ev.kind) {
            case FloatingKind::Crit:
                color = colors::DMG_CRIT;
                break;
            case FloatingKind::DamageReceived:
            case FloatingKind::Death:
                color = colors::DMG_RECEIVED;
                break;
            case FloatingKind::Dodge:
                color = colors::DODGE;
                break;
            default:
                color = colors::WHITE;
                break;
        }
        floating_texts.push_back({ev.x, ev.y, ev.text, color, 0});
    }

    const int ts = config.tile_size;
    auto it = floating_texts.begin();
    while (it != floating_texts.end()) {
        it->age_ms += delta_ms;
        if (it->age_ms >= FLOATING_TEXT_LIFETIME_MS) {
            it = floating_texts.erase(it);
            continue;
        }
        const float t =
            static_cast<float>(it->age_ms) / FLOATING_TEXT_LIFETIME_MS;
        const int rise = static_cast<int>(t * FLOATING_TEXT_RISE_PX);
        const Uint8 alpha = static_cast<Uint8>(255 * (1.0f - t));
        const int cx = cam_offset_x + it->wx * ts + ts / 2;
        const int cy = cam_offset_y + it->wy * ts - ts - rise;
        text.draw_centered(it->text, cx, cy, it->color, alpha);
        ++it;
    }
}

void WorldRenderer::load_effects() {
    std::map<std::string, SDL_Texture*> by_path;
    for (const auto& def : all_effect_defs()) {
        effect_data_map[def.kind] = def.make();
        const std::string path(def.sheet);
        auto found = by_path.find(path);
        if (found != by_path.end()) {
            effect_tex[def.kind] = found->second;
            continue;
        }
        const auto full =
            (std::filesystem::current_path() / config.assets_path / path)
                .lexically_normal();
        SDL_Texture* tex = nullptr;
        if (SDL_Surface* s = IMG_Load(full.string().c_str())) {
            tex = SDL_CreateTextureFromSurface(renderer.Get(), s);
            SDL_FreeSurface(s);
            owned_effect_tex.push_back(tex);
        }
        by_path[path] = tex;
        effect_tex[def.kind] = tex;
    }
}

void WorldRenderer::draw_effects(const ClientGameState& state,
                                 uint32_t delta_ms, int cam_offset_x,
                                 int cam_offset_y) {
    for (const auto& sp : state.get_effect_spawns()) {
        active_effects.push_back({sp.kind, sp.x, sp.y, 0});
    }

    const int ts = config.tile_size;
    auto it = active_effects.begin();
    while (it != active_effects.end()) {
        const EffectData& d = effect_data_map[it->kind];
        const int total = static_cast<int>(d.frames.size()) * d.ms_per_frame;
        it->age_ms += delta_ms;
        if (total <= 0 || it->age_ms >= static_cast<uint32_t>(total)) {
            it = active_effects.erase(it);
            continue;
        }
        int fi = static_cast<int>(it->age_ms / d.ms_per_frame);
        if (fi >= static_cast<int>(d.frames.size())) {
            fi = static_cast<int>(d.frames.size()) - 1;
        }
        const SDL_Rect& src = d.frames[fi];
        SDL_Texture* tex = effect_tex[it->kind];
        if (tex && src.h > 0) {
            const float target_h = ts * EFFECT_HEIGHT_TILES;
            const float scale = target_h / src.h;
            const int dw = static_cast<int>(src.w * scale);
            const int dh = static_cast<int>(target_h);
            const int cx = cam_offset_x + it->wx * ts + ts / 2;
            const int cy = cam_offset_y + it->wy * ts + ts / 2;
            const SDL_Rect dst{cx - dw / 2, cy - dh / 2, dw, dh};
            SDL_RenderCopy(renderer.Get(), tex, &src, &dst);
        }
        ++it;
    }
}

std::string WorldRenderer::local_body_key(const ClientGameState& state,
                                          const std::string& clase_key) const {
    static constexpr std::pair<std::string_view, std::string_view> armaduras[] =
        {
            {items::ARMADURA_CUERO, keys::ARMADURA_CUERO},
            {items::ARMADURA_PLACAS, keys::ARMADURA_PLACAS},
            {items::TUNICA_AZUL, keys::TUNICA_AZUL},
        };
    for (const auto& slot : state.get_inventory()) {
        if (!slot.equipado) {
            continue;
        }
        for (const auto& [item, key] : armaduras) {
            if (slot.item == item) {
                return std::string(key);
            }
        }
    }
    return clase_key;
}

void WorldRenderer::render(const ClientGameState& state, uint32_t delta_ms) {
    update_loaded_map(state);

    int cam_offset_x = 0;
    int cam_offset_y = 0;
    compute_camera(state, cam_offset_x, cam_offset_y);

    draw_layers(cam_offset_x, cam_offset_y);
    draw_floor_items(state, cam_offset_x, cam_offset_y);    
    draw_local(state, delta_ms, cam_offset_x, cam_offset_y);
    draw_others(state, delta_ms, cam_offset_x, cam_offset_y);
    draw_all_creatures(state, delta_ms, cam_offset_x, cam_offset_y);
    draw_effects(state, delta_ms, cam_offset_x, cam_offset_y);
    draw_floating_texts(state, delta_ms, cam_offset_x, cam_offset_y);
}

WorldRenderer::~WorldRenderer() {
    for (SDL_Texture* t : owned_effect_tex) {
        if (t) SDL_DestroyTexture(t);
    }
}
