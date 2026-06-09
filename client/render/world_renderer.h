#ifndef CLIENT_WORLD_RENDERER_H
#define CLIENT_WORLD_RENDERER_H

#include <string>
#include <map>
#include <unordered_map>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#include "config/client_config.h"
#include "game/client_game_state.h"
#include "render/character_animator.h"
#include "render/map_data.h"
#include "render/sprite_registry.h"
#include "render/text_renderer.h"
#include "render/tile_catalog.h"
#include "render/tmx_loader.h"
#include "render/effect_data.h"

struct FloatingText {
    int wx;
    int wy;
    std::string text;
    SDL_Color color;
    uint32_t age_ms;
};

class WorldRenderer {
    private:
    SDL2pp::Renderer& renderer;
    const ClientConfig& config;

    SpriteRegistry registry;
    TextRenderer text;

    std::vector<FloatingText> floating_texts;

    std::unique_ptr<TileCatalog> catalog;
    std::unique_ptr<MapData>     map;

    CharacterAnimator local_anim;
    std::unordered_map<std::string, CharacterAnimator> other_anims;
    std::unordered_map<std::string, CharacterAnimator> creature_anims;

    std::map<EffectKind, EffectData> effect_data_map;
    std::map<EffectKind, SDL_Texture*> effect_tex;
    std::vector<SDL_Texture*> owned_effect_tex;
    std::vector<ActiveEffect> active_effects;

    std::unordered_map<std::string, Uint32> ghost_since;
    
    uint16_t loaded_map_id = 0;

    void load_map_by_id(uint16_t map_id);

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

    void draw_ghost(int world_x, int world_y, protocol::Direction dir,
                    int frame, int cam_offset_x, int cam_offset_y);

    void draw_player(const std::string& nick, bool dead,
                     int world_x, int world_y, protocol::Direction dir,
                     const std::string& sprite_key, const std::string& raza,
                     int frame, int cam_offset_x, int cam_offset_y);

    void draw_creature(int world_x, int world_y,
                       protocol::Direction dir,
                       const std::string& type,
                       int frame,
                       int cam_offset_x, int cam_offset_y);

    void draw_name(const std::string& nick, int world_x, int world_y,
                   int cam_offset_x, int cam_offset_y);

    void draw_floating_texts(const ClientGameState& state, uint32_t delta_ms,
                             int cam_offset_x, int cam_offset_y);

    void draw_effects(const ClientGameState& state, uint32_t delta_ms,
                      int cam_offset_x, int cam_offset_y);

    void load_effects();

    static int dir_to_idx(protocol::Direction dir);

    public:
    WorldRenderer(SDL2pp::Renderer& renderer, const ClientConfig& config);

    void render(const ClientGameState& state, uint32_t delta_ms);

    ~WorldRenderer();
};

#endif