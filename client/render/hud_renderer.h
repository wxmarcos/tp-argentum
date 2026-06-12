#ifndef CLIENT_RENDER_HUD_RENDERER_H
#define CLIENT_RENDER_HUD_RENDERER_H

#include <cstddef>
#include <cstdint>
#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#include "config/client_config.h"
#include "game/client_game_state.h"
#include "render/text_renderer.h"

class HudRenderer {
    private:
    SDL2pp::Renderer& renderer;
    const ClientConfig& config;
    TextRenderer text;
    SDL_Texture* tex_frame;

    void draw_bar(int x, int y, int w, int h, float ratio, SDL_Color fill);

    void draw_stats(const PlayerStats& s);

    void draw_frame();

    void draw_inventory_panel(int panel_x, int panel_y, int panel_h);

    void draw_inventory_slot(const InventorySlotView& slot, size_t idx,
                             int panel_x, int row_y, int row_h);

    public:
    uint32_t last_error_seq = 0;
    Uint32 error_shown_at = 0;

    HudRenderer(SDL2pp::Renderer& renderer, const ClientConfig& config);
    ~HudRenderer();

    void render(const ClientGameState& state);

    void draw_error_toast(const ClientGameState& state);
    
    void draw_inventory(const ClientGameState& state);
};

#endif