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
    const ClientConfig& config;
    TextRenderer text;
    TextRenderer chat_text;
    ItemSpriteRegistry item_sprites;

    void draw_panel();

    void draw_bar(int x, int y, int w, int h, float ratio, SDL_Color fill);

    void draw_player_panel(const ClientGameState& state);

    void draw_header(const PlayerStats& s, const std::string& name, int x,
                     int& y);

    void draw_vitals(const PlayerStats& s, int x, int w, int& y);

    void draw_resources(const PlayerStats& s, int x, int& y);

    void draw_inventory_section(const ClientGameState& state, int x, int w,
                                int y);

    void draw_inventory_slot(const InventorySlotView& slot, int cx, int cy,
                             int cell);

    void draw_chat_panel(const ClientGameState& state, const Console& console);

    public:
    uint32_t last_error_seq = 0;
    Uint32 error_shown_at = 0;

    HudRenderer(SDL2pp::Renderer& renderer, const ClientConfig& config);

    void render(const ClientGameState& state, const Console& console);

    void draw_error_toast(const ClientGameState& state);
};

#endif