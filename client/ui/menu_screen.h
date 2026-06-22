#ifndef CLIENT_UI_MENU_SCREEN_H
#define CLIENT_UI_MENU_SCREEN_H

#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>
#include <array>
#include <string>
#include <string_view>

#include "config/client_config.h"
#include "render/text_renderer.h"
#include "ui/menu_result.h"

class MenuScreen {
private:
    SDL2pp::Renderer& renderer;
    const ClientConfig& config;
    TextRenderer text;

    SDL_Texture* tex_inicio;
    SDL_Texture* tex_login;
    SDL_Texture* tex_create;

    int win_w;
    int win_h;

    SDL_Rect comenzar_rect;
    SDL_Rect nick_box;
    SDL_Rect login_jugar;
    SDL_Rect login_volver;
    std::array<SDL_Rect, 4> raza_rects;
    std::array<SDL_Rect, 4> clase_rects;
    SDL_Rect create_jugar;
    SDL_Rect create_volver;

    SDL_Texture* load_texture(std::string_view rel_path);
    void compute_layout();

    void blit_full(SDL_Texture* tex);

    void draw_selected(const SDL_Rect& r);

    void utf8_backspace(std::string& s) const;

    bool handle_login_event(const SDL_Event& e, std::string& nick,
                            MenuResult& result);

    void draw_login(const std::string& nick, const std::string& error_msg);

    bool handle_create_event(const SDL_Event& e, int& raza_idx, int& clase_idx,
                             MenuResult& result);

    void draw_create(int raza_idx, int clase_idx);

    static bool in_rect(const SDL_Rect& r, int x, int y);

public:
    MenuScreen(SDL2pp::Renderer& renderer, const ClientConfig& config);
    ~MenuScreen();

    MenuScreen(const MenuScreen&) = delete;
    MenuScreen& operator=(const MenuScreen&) = delete;

    MenuResult run_inicio();

    MenuResult run_login(std::string& nick, const std::string& error_msg = "");

    MenuResult run_create(std::string& raza, std::string& clase);
};

#endif
