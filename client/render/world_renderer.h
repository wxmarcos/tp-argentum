#ifndef CLIENT_WORLD_RENDERER_H
#define CLIENT_WORLD_RENDERER_H

#include <string>
#include <unordered_map>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#include "config/client_config.h"
#include "game/client_game_state.h"
#include "render/character_animator.h"
#include "render/map_data.h"
#include "render/sprite_registry.h"
#include "render/tile_catalog.h"
#include "render/tmx_loader.h"

class WorldRenderer {
    private:
    SDL2pp::Renderer& renderer;
    const ClientConfig& config;

    SpriteRegistry registry;

    std::unique_ptr<TileCatalog> catalog;
    std::unique_ptr<MapData>     map;

    CharacterAnimator local_anim;
    std::unordered_map<std::string, CharacterAnimator> other_anims;
    std::unordered_map<std::string, CharacterAnimator> creature_anims;

    void visible_tile_range(int cam_offset_x, int cam_offset_y,
                            int& first_gx, int& last_gx,
                            int& first_gy, int& last_gy) const;

    void draw_map_layer(int layer, int cam_offset_x, int cam_offset_y);

    void draw_character(int world_x, int world_y,
                        protocol::Direction dir,
                        const std::string& sprite_key,
                        const std::string& raza,
                        int frame,
                        int cam_offset_x, int cam_offset_y);

    void draw_creature(int world_x, int world_y,
                       protocol::Direction dir,
                       const std::string& type,
                       int frame,
                       int cam_offset_x, int cam_offset_y);

    static int dir_to_idx(protocol::Direction dir);

    public:
    WorldRenderer(SDL2pp::Renderer& renderer, const ClientConfig& config);

    void render(const ClientGameState& state, uint32_t delta_ms);
};

#endif