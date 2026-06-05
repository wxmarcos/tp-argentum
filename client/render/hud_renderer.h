#ifndef CLIENT_RENDER_HUD_RENDERER_H
#define CLIENT_RENDER_HUD_RENDERER_H

#include <cstdint>
#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#include "config/client_config.h"
#include "game/client_game_state.h"
#include "render/text_renderer.h"

class HudRenderer {
public:
    uint32_t last_error_seq = 0;
    Uint32 error_shown_at = 0;

    HudRenderer(SDL2pp::Renderer& renderer, const ClientConfig& config);

    void render(const ClientGameState& state);

    void draw_error_toast(const ClientGameState& state);

private:
    SDL2pp::Renderer& renderer;
    const ClientConfig& config;
    TextRenderer text;

    void draw_bar(int x, int y, int w, int h, float ratio, SDL_Color fill);
};

#endif