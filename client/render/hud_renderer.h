#ifndef CLIENT_RENDER_HUD_RENDERER_H
#define CLIENT_RENDER_HUD_RENDERER_H

#include <cstddef>
#include <cstdint>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#include "config/client_config.h"
#include "game/client_game_state.h"
#include "render/text_renderer.h"
#include "render/sprites/item_sprite_registry.h"
#include "ui/console.h"

class HudRenderer {
    private:
    SDL2pp::Renderer& renderer;
    SDL_Texture* hud_bg = nullptr;
    SDL_Texture* slot_frame = nullptr;
    SDL_Texture* frame_tex = nullptr;
    int chat_scroll = 0;
    int inv_origin_x = 0;
    int inv_origin_y = 0;
    int inv_cols = 1;
    int inv_count = 0;
    const ClientConfig& config;
    TextRenderer text;
    TextRenderer chat_text;
    ItemSpriteRegistry item_sprites;

    SDL_Texture* load_texture(const std::string& rel_path) const;
    
    void draw_panel();

    void draw_bar(int x, int y, int w, int h, float ratio, SDL_Color fill);

    void draw_player_panel(const ClientGameState& state);

    void draw_header(const PlayerStats& s, const std::string& name, int x,
                     int& y);

    void draw_vitals(const PlayerStats& s, int x, int w, int& y);

    void draw_resources(const PlayerStats& s, int x, int& y);

    void draw_inventory_section(const ClientGameState& state, int x, int w,
                                int y);

    void draw_inventory_slot(const InventorySlotView& slot, int index, int cx,
                         int cy, int cell);

    void draw_chat_panel(const ClientGameState& state, const Console& console);

    void draw_frame();

    public:
    uint32_t last_error_seq = 0;
    Uint32 error_shown_at = 0;

    HudRenderer(SDL2pp::Renderer& renderer, const ClientConfig& config);
    ~HudRenderer();

    void draw_error_toast(const ClientGameState& state);

    void scroll_chat(int delta, int total_msgs);

    int slot_at(int mouse_x, int mouse_y) const;

    void render(const ClientGameState& state, const Console& console);
};

#endif