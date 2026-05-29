#ifndef CLIENT_WORLD_RENDERER_H
#define CLIENT_WORLD_RENDERER_H

#include <SDL2pp/SDL2pp.hh>

#include "config/client_config.h"
#include "game/client_game_state.h"

class WorldRenderer {
    private:
    SDL2pp::Renderer& renderer;
    const ClientConfig& config;

    void draw_grid(int cam_offset_x, int cam_offset_y);

    void draw_character(int world_x, int world_y, protocol::Direction dir,
                        int cam_offset_x, int cam_offset_y, bool is_self);

    public:
    WorldRenderer(SDL2pp::Renderer& renderer, const ClientConfig& config);

    void render(const ClientGameState& state);
};

#endif
