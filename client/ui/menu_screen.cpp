#include "ui/menu_screen.h"

#include <SDL2/SDL_image.h>

#include <filesystem>
#include <iostream>

namespace {
const std::array<const char*, 4> RAZAS = {"humano", "elfo", "enano", "gnomo"};
const std::array<const char*, 4> CLASES = {"mago", "clerigo", "guerrero",
                                           "paladin"};

// inicio.png
const float COMENZAR[4] = {0.43f, 0.84f, 0.19f, 0.11f};
// login.png
const float L_NICK[4] = {0.31f, 0.521f, 0.375f, 0.062f};
const float L_JUGAR[4] = {0.36f, 0.72f, 0.28f, 0.10f};
const float L_VOLVER[4] = {0.045f, 0.875f, 0.15f, 0.062f};
// create_character.png
const float C_RAZA_Y = 0.505f, C_RAZA_H = 0.055f;
const float C_CLASE_Y = 0.611f, C_CLASE_H = 0.055f;
const float C_OPT_X0 = 0.22f, C_OPT_PITCH = 0.147f, C_OPT_W = 0.123f;
const float C_JUGAR[4] = {0.36f, 0.705f, 0.28f, 0.10f};
const float C_VOLVER[4] = {0.045f, 0.86f, 0.15f, 0.062f};

void utf8_backspace(std::string& s) {
    if (s.empty()) return;
    s.pop_back();
    while (!s.empty() &&
           (static_cast<unsigned char>(s.back()) & 0xC0) == 0x80) {
        s.pop_back();
    }
}
}

MenuScreen::MenuScreen(SDL2pp::Renderer& renderer, const ClientConfig& config):
    renderer(renderer),
    config(config),
    text(renderer.Get(),
         (std::filesystem::current_path() / config.font_path).lexically_normal(),
         config.font_size),
    tex_inicio(nullptr),
    tex_login(nullptr),
    tex_create(nullptr),
    win_w(config.window_width),
    win_h(config.window_height) {
    tex_inicio = load_texture("ui/inicio.png");
    tex_login = load_texture("ui/login.png");
    tex_create = load_texture("ui/create_character.png");
    compute_layout();
}

SDL_Texture* MenuScreen::load_texture(const std::string& rel_path) {
    const std::filesystem::path full =
        (std::filesystem::current_path() / config.assets_path / rel_path)
            .lexically_normal();
    SDL_Surface* surface = IMG_Load(full.string().c_str());
    if (!surface) {
        std::cerr << "[MenuScreen] no se pudo cargar: " << full << " ("
                  << IMG_GetError() << ")\n";
        return nullptr;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer.Get(), surface);
    SDL_FreeSurface(surface);
    return tex;
}

void MenuScreen::compute_layout() {
    const SDL2pp::Point out = renderer.GetOutputSize();
    win_w = out.x;
    win_h = out.y;

    auto frac = [&](const float f[4]) {
        return SDL_Rect{static_cast<int>(f[0] * win_w),
                        static_cast<int>(f[1] * win_h),
                        static_cast<int>(f[2] * win_w),
                        static_cast<int>(f[3] * win_h)};
    };

    comenzar_rect = frac(COMENZAR);
    nick_box = frac(L_NICK);
    login_jugar = frac(L_JUGAR);
    login_volver = frac(L_VOLVER);
    create_jugar = frac(C_JUGAR);
    create_volver = frac(C_VOLVER);

    for (int i = 0; i < 4; ++i) {
        const float x = C_OPT_X0 + i * C_OPT_PITCH;
        raza_rects[i] = {static_cast<int>(x * win_w),
                         static_cast<int>(C_RAZA_Y * win_h),
                         static_cast<int>(C_OPT_W * win_w),
                         static_cast<int>(C_RAZA_H * win_h)};
        clase_rects[i] = {static_cast<int>(x * win_w),
                          static_cast<int>(C_CLASE_Y * win_h),
                          static_cast<int>(C_OPT_W * win_w),
                          static_cast<int>(C_CLASE_H * win_h)};
    }
}

bool MenuScreen::in_rect(const SDL_Rect& r, int x, int y) {
    return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
}

void MenuScreen::blit_full(SDL_Texture* tex) {
    const SDL_Rect full{0, 0, win_w, win_h};
    if (tex) {
        SDL_RenderCopy(renderer.Get(), tex, nullptr, &full);
    } else {
        renderer.SetDrawColor(20, 20, 25, 255);
        renderer.FillRect(SDL2pp::Rect(0, 0, win_w, win_h));
    }
}

void MenuScreen::draw_selected(const SDL_Rect& r) {
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(255, 200, 80, 60);
    renderer.FillRect(SDL2pp::Rect(r.x, r.y, r.w, r.h));
    renderer.SetDrawColor(255, 210, 110, 255);
    renderer.DrawRect(SDL2pp::Rect(r.x, r.y, r.w, r.h));
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
}

MenuResult MenuScreen::run_inicio() {
    while (true) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return MenuResult::QUIT;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
                return MenuResult::QUIT;
            if (e.type == SDL_MOUSEBUTTONDOWN &&
                e.button.button == SDL_BUTTON_LEFT &&
                in_rect(comenzar_rect, e.button.x, e.button.y)) {
                return MenuResult::NEXT;
            }
        }
        renderer.SetDrawColor(0, 0, 0, 255);
        renderer.Clear();
        blit_full(tex_inicio);
        renderer.Present();
        SDL_Delay(16);
    }
}

MenuResult MenuScreen::run_login(std::string& nick) {
    SDL_StartTextInput();
    MenuResult result = MenuResult::BACK;
    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                result = MenuResult::QUIT;
                running = false;
                break;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    result = MenuResult::BACK;
                    running = false;
                } else if (e.key.keysym.sym == SDLK_BACKSPACE) {
                    utf8_backspace(nick);
                } else if (e.key.keysym.sym == SDLK_RETURN && !nick.empty()) {
                    result = MenuResult::NEXT;
                    running = false;
                }
            } else if (e.type == SDL_TEXTINPUT) {
                if (nick.size() < 16) nick += e.text.text;
            } else if (e.type == SDL_MOUSEBUTTONDOWN &&
                       e.button.button == SDL_BUTTON_LEFT) {
                if (in_rect(login_volver, e.button.x, e.button.y)) {
                    result = MenuResult::BACK;
                    running = false;
                } else if (in_rect(login_jugar, e.button.x, e.button.y) &&
                           !nick.empty()) {
                    result = MenuResult::NEXT;
                    running = false;
                }
            }
        }
        renderer.SetDrawColor(0, 0, 0, 255);
        renderer.Clear();
        blit_full(tex_login);
        const int ty = nick_box.y + (nick_box.h - text.line_height()) / 2;
        text.draw(nick + "_", nick_box.x + 12, ty,
                  SDL_Color{255, 255, 255, 255});
        renderer.Present();
        SDL_Delay(16);
    }
    SDL_StopTextInput();
    return result;
}

MenuResult MenuScreen::run_create(std::string& raza, std::string& clase) {
    int raza_idx = -1;
    int clase_idx = -1;
    MenuResult result = MenuResult::BACK;
    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                result = MenuResult::QUIT;
                running = false;
                break;
            }
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                result = MenuResult::BACK;
                running = false;
            } else if (e.type == SDL_MOUSEBUTTONDOWN &&
                       e.button.button == SDL_BUTTON_LEFT) {
                const int mx = e.button.x;
                const int my = e.button.y;
                for (int i = 0; i < 4; ++i) {
                    if (in_rect(raza_rects[i], mx, my)) raza_idx = i;
                    if (in_rect(clase_rects[i], mx, my)) clase_idx = i;
                }
                if (in_rect(create_volver, mx, my)) {
                    result = MenuResult::BACK;
                    running = false;
                } else if (in_rect(create_jugar, mx, my) && raza_idx >= 0 &&
                           clase_idx >= 0) {
                    result = MenuResult::NEXT;
                    running = false;
                }
            }
        }
        renderer.SetDrawColor(0, 0, 0, 255);
        renderer.Clear();
        blit_full(tex_create);
        if (raza_idx >= 0) draw_selected(raza_rects[raza_idx]);
        if (clase_idx >= 0) draw_selected(clase_rects[clase_idx]);
        renderer.Present();
        SDL_Delay(16);
    }
    if (result == MenuResult::NEXT) {
        raza = RAZAS[raza_idx];
        clase = CLASES[clase_idx];
    }
    return result;
}

MenuScreen::~MenuScreen() {
    if (tex_inicio) SDL_DestroyTexture(tex_inicio);
    if (tex_login) SDL_DestroyTexture(tex_login);
    if (tex_create) SDL_DestroyTexture(tex_create);
}
