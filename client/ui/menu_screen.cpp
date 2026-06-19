#include "ui/menu_screen.h"

#include <SDL2/SDL_image.h>

#include <array>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <string_view>

#include "game/entity_keys.h"
#include "render/colors.h"
#include "render/asset_paths.h"

static constexpr int OPTION_COUNT = 4;
static constexpr size_t MAX_NICK_LEN = 16;
static constexpr int NICK_TEXT_PAD = 24;
static constexpr int FRAME_DELAY_MS = 16;
static constexpr int ERROR_MSG_GAP = 16;

static constexpr std::array<std::string_view, 4> RAZAS = {
    keys::HUMANO, keys::ELFO, keys::ENANO, keys::GNOMO};
static constexpr std::array<std::string_view, 4> CLASES = {
    keys::MAGO, keys::CLERIGO, keys::GUERRERO, keys::PALADIN};

// inicio.png
static constexpr float COMENZAR[4] = {0.34f, 0.835f, 0.33f, 0.1f};

// login.png
static constexpr float L_NICK[4] = {0.31f, 0.521f, 0.375f, 0.062f};
static constexpr float L_JUGAR[4] = {0.375f, 0.74f, 0.25f, 0.10f};
static constexpr float L_VOLVER[4] = {0.045f, 0.87f, 0.15f, 0.062f};

// create_character.png
static constexpr float C_RAZA_Y = 0.505f, C_RAZA_H = 0.055f;
static constexpr float C_CLASE_Y = 0.611f, C_CLASE_H = 0.055f;
static constexpr float C_OPT_X[4] = {0.225f, 0.374f, 0.51f, 0.65f};
static constexpr float C_OPT_W[4] = {0.133f, 0.123f, 0.125f, 0.126f};
static constexpr float C_JUGAR[4] = {0.375f, 0.725f, 0.25f, 0.10f};
static constexpr float C_VOLVER[4] = {0.045f, 0.87f, 0.15f, 0.062f};


MenuScreen::MenuScreen(SDL2pp::Renderer& renderer, const ClientConfig& config):
        renderer(renderer),
        config(config),
        text(renderer.Get(),
             (std::filesystem::current_path() / config.font_path)
                 .lexically_normal(),
             config.font_size),
        tex_inicio(nullptr),
        tex_login(nullptr),
        tex_create(nullptr),
        win_w(config.window_width),
        win_h(config.window_height) {
    tex_inicio = load_texture(assets::UI_INICIO);
    tex_login = load_texture(assets::UI_LOGIN);
    tex_create = load_texture(assets::UI_CREATE);
    compute_layout();
}

SDL_Texture* MenuScreen::load_texture(std::string_view rel_path) {
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
    win_w = config.window_width;
    win_h = config.window_height;

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

    for (int i = 0; i < OPTION_COUNT; ++i) {
        raza_rects[i] = {static_cast<int>(C_OPT_X[i] * win_w),
                         static_cast<int>(C_RAZA_Y * win_h),
                         static_cast<int>(C_OPT_W[i] * win_w),
                         static_cast<int>(C_RAZA_H * win_h)};
        clase_rects[i] = {static_cast<int>(C_OPT_X[i] * win_w),
                          static_cast<int>(C_CLASE_Y * win_h),
                          static_cast<int>(C_OPT_W[i] * win_w),
                          static_cast<int>(C_CLASE_H * win_h)};
    }
}

bool MenuScreen::in_rect(const SDL_Rect& r, int x, int y) {
    return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
}

void MenuScreen::utf8_backspace(std::string& s) const {
    if (s.empty()) return;
    s.pop_back();
    while (!s.empty() &&
           (static_cast<unsigned char>(s.back()) & 0xC0) == 0x80) {
        s.pop_back();
    }
}

void MenuScreen::blit_full(SDL_Texture* tex) {
    const SDL_Rect full{0, 0, win_w, win_h};
    if (tex) {
        SDL_RenderCopy(renderer.Get(), tex, nullptr, &full);
    } else {
        renderer.SetDrawColor(colors::MENU_BG.r, colors::MENU_BG.g,
                              colors::MENU_BG.b, colors::MENU_BG.a);
        renderer.FillRect(SDL2pp::Rect(0, 0, win_w, win_h));
    }
}

void MenuScreen::draw_selected(const SDL_Rect& r) {
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(colors::SELECT_FILL.r, colors::SELECT_FILL.g,
                          colors::SELECT_FILL.b, colors::SELECT_FILL.a);
    renderer.FillRect(SDL2pp::Rect(r.x, r.y, r.w, r.h));
    renderer.SetDrawColor(colors::SELECT_BORDER.r, colors::SELECT_BORDER.g,
                          colors::SELECT_BORDER.b, colors::SELECT_BORDER.a);
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
        SDL_Delay(FRAME_DELAY_MS);
    }
}

bool MenuScreen::handle_login_event(const SDL_Event& e, std::string& nick,
                                    MenuResult& result) {
    if (e.type == SDL_QUIT) {
        result = MenuResult::QUIT;
        return false;
    }
    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_ESCAPE) {
            result = MenuResult::BACK;
            return false;
        }
        if (e.key.keysym.sym == SDLK_BACKSPACE) {
            utf8_backspace(nick);
        } else if (e.key.keysym.sym == SDLK_RETURN && !nick.empty()) {
            result = MenuResult::NEXT;
            return false;
        }
        return true;
    }
    if (e.type == SDL_TEXTINPUT) {
        if (nick.size() < MAX_NICK_LEN) nick += e.text.text;
        return true;
    }
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        if (in_rect(login_volver, e.button.x, e.button.y)) {
            result = MenuResult::BACK;
            return false;
        }
        if (in_rect(login_jugar, e.button.x, e.button.y) && !nick.empty()) {
            result = MenuResult::NEXT;
            return false;
        }
    }
    return true;
}

void MenuScreen::draw_login(const std::string& nick,
                            const std::string& error_msg) {
    renderer.SetDrawColor(0, 0, 0, 255);
    renderer.Clear();
    blit_full(tex_login);
    const int ty = nick_box.y + (nick_box.h - text.line_height()) / 2;
    text.draw(nick + "_", nick_box.x + NICK_TEXT_PAD, ty, colors::WHITE);

    if (!error_msg.empty()) {
        const int ey = nick_box.y + nick_box.h + ERROR_MSG_GAP;
        text.draw_centered(error_msg, config.window_width / 2, ey,
                           colors::ERROR_TEXT);
    }
    renderer.Present();
}

MenuResult MenuScreen::run_login(std::string& nick,
                                 const std::string& error_msg) {
    SDL_StartTextInput();
    MenuResult result = MenuResult::BACK;
    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            running = handle_login_event(e, nick, result);
            if (!running) break;
        }
        draw_login(nick, error_msg);
        SDL_Delay(FRAME_DELAY_MS);
    }
    SDL_StopTextInput();
    return result;
}

bool MenuScreen::handle_create_event(const SDL_Event& e, int& raza_idx,
                                     int& clase_idx, MenuResult& result) {
    if (e.type == SDL_QUIT) {
        result = MenuResult::QUIT;
        return false;
    }
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
        result = MenuResult::BACK;
        return false;
    }
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        const int mx = e.button.x;
        const int my = e.button.y;
        for (int i = 0; i < OPTION_COUNT; ++i) {
            if (in_rect(raza_rects[i], mx, my)) raza_idx = i;
            if (in_rect(clase_rects[i], mx, my)) clase_idx = i;
        }
        if (in_rect(create_volver, mx, my)) {
            result = MenuResult::BACK;
            return false;
        }
        if (in_rect(create_jugar, mx, my) && raza_idx >= 0 && clase_idx >= 0) {
            result = MenuResult::NEXT;
            return false;
        }
    }
    return true;
}

void MenuScreen::draw_create(int raza_idx, int clase_idx) {
    renderer.SetDrawColor(0, 0, 0, 255);
    renderer.Clear();
    blit_full(tex_create);
    if (raza_idx >= 0) draw_selected(raza_rects[raza_idx]);
    if (clase_idx >= 0) draw_selected(clase_rects[clase_idx]);
    renderer.Present();
}

MenuResult MenuScreen::run_create(std::string& raza, std::string& clase) {
    int raza_idx = -1;
    int clase_idx = -1;
    MenuResult result = MenuResult::BACK;
    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            running = handle_create_event(e, raza_idx, clase_idx, result);
            if (!running) break;
        }
        draw_create(raza_idx, clase_idx);
        SDL_Delay(FRAME_DELAY_MS);
    }
    if (result == MenuResult::NEXT) {
        raza.assign(RAZAS[raza_idx]);
        clase.assign(CLASES[clase_idx]);
    }
    return result;
}

MenuScreen::~MenuScreen() {
    if (tex_inicio) SDL_DestroyTexture(tex_inicio);
    if (tex_login) SDL_DestroyTexture(tex_login);
    if (tex_create) SDL_DestroyTexture(tex_create);
}
