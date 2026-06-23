#ifndef CLIENT_WORLD_RENDERER_H
#define CLIENT_WORLD_RENDERER_H

#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "config/client_config.h"
#include "game/client_game_state.h"
#include "render/effects/effect_data.h"
#include "render/map/map_data.h"
#include "render/map/tile_catalog.h"
#include "render/map/tmx_loader.h"
#include "render/sprites/character_animator.h"
#include "render/sprites/item_sprite_registry.h"
#include "render/sprites/sprite_registry.h"
#include "render/sprites/weapon_sprite_registry.h"
#include "render/text_renderer.h"

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
    ItemSpriteRegistry item_sprites;
    WeaponSpriteRegistry weapon_sprites;

    std::vector<FloatingText> floating_texts;

    std::unique_ptr<TileCatalog> catalog;
    std::unique_ptr<MapData> map;

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
    void update_loaded_map(const ClientGameState& state);

    void compute_camera(const ClientGameState& state, int& cam_offset_x,
                        int& cam_offset_y) const;

    void visible_tile_range(int cam_offset_x, int cam_offset_y, int& first_gx,
                            int& last_gx, int& first_gy, int& last_gy) const;

    void draw_layers(int cam_offset_x, int cam_offset_y);

    void draw_map_layer(int layer, int cam_offset_x, int cam_offset_y);

    void draw_fallback_layer(int cam_offset_x, int cam_offset_y);

    void blit_tile(SDL_Texture* tex, const SDL_Rect& src, const SDL_Rect& dst,
                   uint8_t flip);

    void draw_local(const ClientGameState& state, uint32_t delta_ms,
                    int cam_offset_x, int cam_offset_y);

    void draw_others(const ClientGameState& state, uint32_t delta_ms,
                     int cam_offset_x, int cam_offset_y);

    void draw_all_creatures(const ClientGameState& state, uint32_t delta_ms,
                            int cam_offset_x, int cam_offset_y);

    void draw_character(int world_x, int world_y, protocol::Direction dir,
                        const std::string& sprite_key, const std::string& raza,
                        int frame, int cam_offset_x, int cam_offset_y,
                        const std::string& weapon_name = "",
                        const std::string& helmet_key = "",
                        const std::string& shield_name = "");

    void draw_body(const std::string& sprite_key, int dir_idx, int frame,
                   int px, int py, int body_top, int body_h);

    void draw_head(const std::string& sprite_key, const std::string& raza,
                   int dir_idx, int px, int body_top, int body_scale);

    void draw_helmet(const std::string& helmet_key, const std::string& raza,
                     const std::string& sprite_key, int dir_idx, int px,
                     int body_top, int body_scale);

    void draw_ghost(int world_x, int world_y, protocol::Direction dir,
                    int frame, int cam_offset_x, int cam_offset_y);

    void draw_player(const std::string& nick, bool dead, int world_x,
                     int world_y, protocol::Direction dir,
                     const std::string& sprite_key, const std::string& raza,
                     int frame, int cam_offset_x, int cam_offset_y,
                     const std::string& weapon_name = "",
                     const std::string& helmet_key = "",
                     const std::string& shield_name = "");

    void draw_creature(int world_x, int world_y, protocol::Direction dir,
                       const std::string& type, int frame, int cam_offset_x,
                       int cam_offset_y);

    void draw_name(const std::string& nick, int world_x, int world_y,
                   int cam_offset_x, int cam_offset_y,
                   const std::string& raza = "");

    void draw_floor_items(const ClientGameState& state, int cam_offset_x,
                          int cam_offset_y);

    void draw_weapon(const std::string& weapon_name, int dir_idx, int frame,
                     int px, int body_top, int body_h);

    void draw_meditation_effect(int world_x, int world_y, int cam_offset_x,
                                int cam_offset_y);

    void draw_floating_texts(const ClientGameState& state, uint32_t delta_ms,
                             int cam_offset_x, int cam_offset_y);

    void draw_effects(const ClientGameState& state, uint32_t delta_ms,
                      int cam_offset_x, int cam_offset_y);

    void load_effects();

    int body_scale_pct(const std::string& raza) const;

    int head_scale_pct(const std::string& raza) const;

    int creature_scale_pct(const std::string& type) const;

    static int dir_to_idx(protocol::Direction dir);

    std::string weapon_name_for(
        const std::vector<InventorySlotView>& inv) const;

    std::string body_key_for(const std::vector<InventorySlotView>& inv,
                             const std::string& clase_key) const;

    std::string helmet_key_for(const std::vector<InventorySlotView>& inv) const;

    std::string shield_name_for(
        const std::vector<InventorySlotView>& inv) const;

    bool is_shield(const std::string& item) const;

public:
    WorldRenderer(SDL2pp::Renderer& renderer, const ClientConfig& config);

    bool screen_to_tile(const ClientGameState& state, int mouse_x, int mouse_y,
                        uint16_t& tile_x, uint16_t& tile_y) const;

    void render(const ClientGameState& state, uint32_t delta_ms);

    void load_map(const std::string& map_name);

    ~WorldRenderer();
};

#endif
