#ifndef CLIENT_WORLD_RENDERER_H
#define CLIENT_WORLD_RENDERER_H

#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>
#include <string>
#include <unordered_map>

#include "config/client_config.h"
#include "game/client_game_state.h"
#include "render/character_animator.h"
#include "render/sprite_registry.h"

static constexpr SDL_Rect TILE_GRASS_SRC = {224, 512, 32, 32};

class WorldRenderer {
private:
    SDL2pp::Renderer& renderer;
    const ClientConfig& config;

    SpriteRegistry registry;

    CharacterAnimator local_anim;
    std::unordered_map<std::string, CharacterAnimator> other_anims;
    std::unordered_map<std::string, CharacterAnimator> creature_anims;

    void visible_tile_range(int cam_offset_x, int cam_offset_y, int& first_gx,
                            int& last_gx, int& first_gy, int& last_gy) const;

    void draw_tiles(int cam_offset_x, int cam_offset_y);

    void draw_character(int world_x, int world_y, protocol::Direction dir,
                        const std::string& sprite_key, const std::string& raza,
                        int frame, int cam_offset_x, int cam_offset_y);

    void draw_creature(int world_x, int world_y, protocol::Direction dir,
                       const std::string& type, int frame, int cam_offset_x,
                       int cam_offset_y);

    static int dir_to_idx(protocol::Direction dir);

public:
    WorldRenderer(SDL2pp::Renderer& renderer, const ClientConfig& config);

    void render(const ClientGameState& state, uint32_t delta_ms);
};

#endif
