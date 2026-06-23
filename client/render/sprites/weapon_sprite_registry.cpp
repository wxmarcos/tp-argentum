#include "render/sprites/weapon_sprite_registry.h"

#include <SDL2/SDL_image.h>

#include <filesystem>

#include "render/asset_paths.h"

static constexpr WeaponDirAdjust DEFAULT_ADJUST_S{8, 20, false};
static constexpr WeaponDirAdjust DEFAULT_ADJUST_N{-8, 20, false};
static constexpr WeaponDirAdjust DEFAULT_ADJUST_E{10, 20, false};
static constexpr WeaponDirAdjust DEFAULT_ADJUST_W{-10, 20, false};

WeaponSpriteRegistry::WeaponSpriteRegistry(SDL2pp::Renderer& renderer,
                                           const ClientConfig& config):
    renderer(renderer),
    config(config) {
    load_defs();
}

WeaponSpriteRegistry::~WeaponSpriteRegistry() {
    for (SDL_Texture* t : owned) {
        if (t) {
            SDL_DestroyTexture(t);
        }
    }
}

void WeaponSpriteRegistry::load_defs() {
    {
        const SDL_Rect s[WALK_FRAME_COUNT] = {
            {16, 4, 7, 24}, {54, 2, 8, 24}, {92, 4, 7, 24}, {130, 2, 7, 24}};
        const SDL_Rect e[WALK_FRAME_COUNT] = {{15, 37, 19, 17},
                                              {53, 35, 20, 16},
                                              {91, 37, 19, 17},
                                              {129, 35, 18, 18}};
        const SDL_Rect w[WALK_FRAME_COUNT] = {{3, 67, 19, 17},
                                              {42, 65, 18, 18},
                                              {79, 67, 19, 17},
                                              {116, 65, 20, 16}};
        register_weapon("Espada", std::string(assets::ESPADA_ANIM), s, e, w);
    }
    {
        const SDL_Rect s[WALK_FRAME_COUNT] = {{16, 7, 10, 19},
                                              {53, 5, 10, 19},
                                              {92, 7, 10, 19},
                                              {131, 5, 10, 19}};
        const SDL_Rect e[WALK_FRAME_COUNT] = {{20, 40, 16, 16},
                                              {58, 37, 15, 17},
                                              {96, 40, 16, 16},
                                              {134, 39, 17, 15}};
        const SDL_Rect w[WALK_FRAME_COUNT] = {{4, 70, 16, 16},
                                              {43, 67, 15, 17},
                                              {80, 70, 16, 16},
                                              {117, 69, 17, 15}};
        register_weapon("Hacha", std::string(assets::HACHA_ANIM), s, e, w);
    }
    {
        const SDL_Rect s[WALK_FRAME_COUNT] = {
            {16, 7, 9, 19}, {55, 5, 9, 19}, {92, 7, 9, 19}, {129, 5, 9, 19}};
        const SDL_Rect e[WALK_FRAME_COUNT] = {{17, 39, 17, 15},
                                              {55, 37, 17, 14},
                                              {93, 39, 17, 15},
                                              {131, 37, 17, 16}};
        const SDL_Rect w[WALK_FRAME_COUNT] = {{7, 69, 17, 15},
                                              {45, 67, 17, 16},
                                              {83, 69, 17, 15},
                                              {120, 67, 18, 15}};
        register_weapon("Martillo", std::string(assets::MARTILLO_ANIM), s, e,
                        w);
    }
    {
        const SDL_Rect s[WALK_FRAME_COUNT] = {
            {15, 4, 9, 39}, {51, 2, 9, 39}, {91, 4, 9, 39}, {132, 2, 9, 40}};
        const SDL_Rect e[WALK_FRAME_COUNT] = {{4, 54, 30, 29},
                                              {42, 50, 29, 31},
                                              {80, 54, 30, 29},
                                              {118, 54, 32, 27}};
        const SDL_Rect w[WALK_FRAME_COUNT] = {{6, 98, 30, 29},
                                              {45, 94, 29, 31},
                                              {82, 98, 30, 29},
                                              {118, 98, 32, 27}};
        register_weapon("Vara de fresno",
                        std::string(assets::VARADEFRESNO_ANIM), s, e, w);
    }
    {
        const SDL_Rect s[WALK_FRAME_COUNT] = {
            {18, 5, 6, 35}, {54, 3, 8, 35}, {94, 5, 6, 35}, {134, 2, 4, 36}};
        const SDL_Rect e[WALK_FRAME_COUNT] = {{7, 50, 24, 25},
                                              {45, 47, 22, 26},
                                              {83, 50, 24, 25},
                                              {121, 50, 26, 23}};
        const SDL_Rect w[WALK_FRAME_COUNT] = {{9, 90, 24, 25},
                                              {49, 87, 22, 26},
                                              {85, 90, 24, 25},
                                              {121, 90, 26, 23}};
        register_weapon("Flauta elfica", std::string(assets::FLAUTAELFICA_ANIM),
                        s, e, w);
    }
    {
        const SDL_Rect s[WALK_FRAME_COUNT] = {{15, 4, 10, 39},
                                              {51, 2, 10, 39},
                                              {91, 4, 10, 39},
                                              {131, 2, 10, 39}};
        const SDL_Rect e[WALK_FRAME_COUNT] = {{6, 56, 26, 26},
                                              {44, 52, 25, 28},
                                              {82, 56, 26, 26},
                                              {120, 55, 28, 25}};
        const SDL_Rect w[WALK_FRAME_COUNT] = {{8, 100, 26, 26},
                                              {47, 96, 25, 28},
                                              {84, 100, 26, 26},
                                              {120, 99, 28, 25}};
        register_weapon("Baculo nudoso", std::string(assets::BACULONUDOSO_ANIM),
                        s, e, w);
    }
    {
        const SDL_Rect s[WALK_FRAME_COUNT] = {{11, 4, 18, 40},
                                              {47, 3, 18, 39},
                                              {87, 4, 18, 40},
                                              {127, 2, 18, 40}};
        const SDL_Rect e[WALK_FRAME_COUNT] = {{5, 55, 28, 27},
                                              {43, 51, 27, 29},
                                              {81, 55, 28, 27},
                                              {119, 54, 30, 26}};
        const SDL_Rect w[WALK_FRAME_COUNT] = {{7, 99, 28, 27},
                                              {46, 95, 27, 29},
                                              {83, 99, 28, 27},
                                              {119, 98, 30, 26}};
        register_weapon("Baculo engarzado",
                        std::string(assets::BACULOENGARZADO_ANIM), s, e, w);
    }
    {
        const SDL_Rect s[WALK_FRAME_COUNT] = {{5, 14, 29, 11},
                                              {43, 12, 29, 11},
                                              {81, 14, 29, 11},
                                              {119, 12, 29, 11}};
        const SDL_Rect e[WALK_FRAME_COUNT] = {{13, 38, 11, 29},
                                              {51, 36, 12, 29},
                                              {89, 38, 11, 29},
                                              {127, 36, 11, 29}};
        const SDL_Rect w[WALK_FRAME_COUNT] = {{12, 72, 11, 29},
                                              {49, 70, 12, 29},
                                              {88, 72, 11, 29},
                                              {126, 70, 11, 29}};
        register_weapon("Arco simple", std::string(assets::ARCOSIMPLE_ANIM), s,
                        e, w);
    }
    {
        const SDL_Rect s[WALK_FRAME_COUNT] = {{7, 10, 25, 12},
                                              {45, 7, 25, 14},
                                              {83, 10, 25, 12},
                                              {121, 8, 25, 12}};
        const SDL_Rect e[WALK_FRAME_COUNT] = {{13, 34, 12, 25},
                                              {51, 32, 13, 25},
                                              {89, 34, 12, 25},
                                              {127, 32, 12, 25}};
        const SDL_Rect w[WALK_FRAME_COUNT] = {{13, 64, 12, 25},
                                              {50, 62, 13, 25},
                                              {89, 64, 12, 25},
                                              {127, 62, 12, 25}};
        register_weapon("Arco compuesto",
                        std::string(assets::ARCOCOMPUESTO_ANIM), s, e, w);
    }
    {
        const SDL_Rect s[WALK_FRAME_COUNT] = {{10, 4, 19, 24},
                                              {48, 3, 19, 24},
                                              {86, 4, 19, 24},
                                              {124, 3, 19, 24}};
        const SDL_Rect e[WALK_FRAME_COUNT] = {{14, 37, 12, 19},
                                              {52, 36, 12, 19},
                                              {90, 37, 12, 19},
                                              {128, 36, 12, 19}};
        const SDL_Rect w[WALK_FRAME_COUNT] = {{14, 67, 12, 19},
                                              {52, 66, 12, 19},
                                              {90, 67, 12, 19},
                                              {128, 66, 12, 19}};
        register_weapon("Escudo de tortuga",
                        std::string(assets::ESCUDOTORTUGA_ANIM), s, e, w);
    }
    {
        const SDL_Rect s[WALK_FRAME_COUNT] = {{10, 3, 20, 27},
                                              {48, 2, 20, 27},
                                              {86, 3, 20, 27},
                                              {124, 2, 20, 27}};
        const SDL_Rect e[WALK_FRAME_COUNT] = {{14, 35, 12, 22},
                                              {52, 34, 12, 22},
                                              {90, 35, 12, 22},
                                              {128, 34, 12, 22}};
        const SDL_Rect w[WALK_FRAME_COUNT] = {{14, 65, 12, 22},
                                              {52, 64, 12, 22},
                                              {90, 65, 12, 22},
                                              {128, 64, 12, 22}};
        register_weapon("Escudo de hierro",
                        std::string(assets::ESCUDOHIERRO_ANIM), s, e, w);
    }

    set_adjust("Espada", {-10, 24, false}, DEFAULT_ADJUST_N, {10, 30, false},
               {-10, 30, false});
    set_adjust("Hacha", {-10, 22, false}, DEFAULT_ADJUST_N, {10, 22, false},
               {-10, 22, false});
    set_adjust("Martillo", {-10, 26, false}, DEFAULT_ADJUST_N, {10, 30, false},
               {-10, 30, false});
    set_adjust("Vara de fresno", {-8, 10, false}, DEFAULT_ADJUST_N,
               {6, 16, false}, {-6, 16, false});
    set_adjust("Flauta elfica", {-8, 10, false}, DEFAULT_ADJUST_N,
               {6, 16, false}, {-6, 16, false});
    set_adjust("Baculo nudoso", {-8, 10, false}, DEFAULT_ADJUST_N,
               {6, 16, false}, {-6, 16, false});
    set_adjust("Baculo engarzado", {-8, 10, false}, DEFAULT_ADJUST_N,
               {6, 16, false}, {-6, 16, false});
    set_adjust("Arco simple", {-14, 25, false}, DEFAULT_ADJUST_N, {1, 16, true},
               {-1, 16, true});
    set_adjust("Arco compuesto", {-15, 26, false}, DEFAULT_ADJUST_N,
               {2, 16, true}, {-2, 16, true});
    set_adjust("Escudo de tortuga", {10, 18, false}, DEFAULT_ADJUST_N,
               {5, 18, false}, {-5, 18, false});
    set_adjust("Escudo de hierro", {10, 18, false}, DEFAULT_ADJUST_N,
               {5, 18, false}, {-5, 18, false});
}

SDL_Texture* WeaponSpriteRegistry::load_texture(const std::string& rel_path) {
    auto cached = tex_by_path.find(rel_path);
    if (cached != tex_by_path.end()) {
        return cached->second;
    }
    const auto full =
        (std::filesystem::current_path() / config.assets_path / rel_path)
            .lexically_normal();
    SDL_Texture* tex = nullptr;
    if (SDL_Surface* s = IMG_Load(full.string().c_str())) {
        tex = SDL_CreateTextureFromSurface(renderer.Get(), s);
        SDL_FreeSurface(s);
        if (tex) {
            owned.push_back(tex);
        }
    }
    tex_by_path[rel_path] = tex;
    return tex;
}

void WeaponSpriteRegistry::register_weapon(
    const std::string& name, const std::string& sheet,
    const SDL_Rect (&south)[WALK_FRAME_COUNT],
    const SDL_Rect (&east)[WALK_FRAME_COUNT],
    const SDL_Rect (&west)[WALK_FRAME_COUNT]) {
    SDL_Texture* tex = load_texture(sheet);
    if (!tex) {
        return;
    }
    WeaponSprite ws;
    ws.tex = tex;
    for (int f = 0; f < WALK_FRAME_COUNT; ++f) {
        ws.rects[0][f] = south[f];
        ws.rects[1][f] = south[f];
        ws.rects[2][f] = east[f];
        ws.rects[3][f] = west[f];
    }
    ws.adjust[0] = DEFAULT_ADJUST_S;
    ws.adjust[1] = DEFAULT_ADJUST_N;
    ws.adjust[2] = DEFAULT_ADJUST_E;
    ws.adjust[3] = DEFAULT_ADJUST_W;
    weapons[name] = ws;
}

void WeaponSpriteRegistry::set_adjust(const std::string& name,
                                      const WeaponDirAdjust& s,
                                      const WeaponDirAdjust& n,
                                      const WeaponDirAdjust& e,
                                      const WeaponDirAdjust& w) {
    auto it = weapons.find(name);
    if (it == weapons.end()) {
        return;
    }
    it->second.adjust[0] = s;
    it->second.adjust[1] = n;
    it->second.adjust[2] = e;
    it->second.adjust[3] = w;
}

const WeaponSprite* WeaponSpriteRegistry::find(const std::string& name) const {
    auto it = weapons.find(name);
    return (it != weapons.end()) ? &it->second : nullptr;
}
