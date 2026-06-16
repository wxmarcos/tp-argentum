#include "render/sprites/weapon_sprite_registry.h"

#include <filesystem>

#include <SDL2/SDL_image.h>

#include "render/asset_paths.h"

static constexpr WeaponDirAdjust DEFAULT_ADJUST_S{8, 20, false};
static constexpr WeaponDirAdjust DEFAULT_ADJUST_N{-8, 20, false};
static constexpr WeaponDirAdjust DEFAULT_ADJUST_E{10, 20, false};
static constexpr WeaponDirAdjust DEFAULT_ADJUST_W{-10, 20, false};

WeaponSpriteRegistry::WeaponSpriteRegistry(SDL2pp::Renderer& renderer,
                                           const ClientConfig& config):
        renderer(renderer), config(config) {
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
    register_weapon("Espada", std::string(assets::ESPADA),
                    {34, 19, 19, 17}, {98, 67, 7, 24}, {83, 98, 7, 20},
                    {96, 146, 11, 23});
    register_weapon("Hacha", std::string(assets::HACHA),
                    {59, 8, 19, 16}, {86, 56, 17, 16}, {27, 107, 15, 16},
                    {61, 154, 16, 15});
    register_weapon("Martillo", std::string(assets::MARTILLO),
                    {84, 23, 8, 19}, {96, 65, 10, 20}, {109, 98, 11, 20},
                    {91, 146, 16, 19});
    register_weapon("Vara de fresno", std::string(assets::VARADEFRESNO),
                    {83, 7, 20, 19}, {1, 51, 20, 19}, {55, 106, 17, 17},
                    {63, 153, 17, 17});
    register_weapon("Flauta elfica", std::string(assets::FLAUTAELFICA),
                    {28, 5, 13, 39}, {40, 48, 9, 39}, {114, 97, 9, 40},
                    {68, 146, 10, 39});
    register_weapon("Baculo nudoso", std::string(assets::BACULONUDOSO),
                    {1, 4, 11, 40}, {120, 50, 13, 39}, {3, 99, 13, 39},
                    {59, 147, 16, 38});
    register_weapon("Baculo engarzado", std::string(assets::BACULOENGARZADO),
                    {0, 3, 18, 40}, {90, 52, 17, 40}, {110, 97, 18, 40},
                    {114, 144, 18, 40});
    register_weapon("Arco simple", std::string(assets::ARCOSIMPLE),
                    {55, 0, 17, 29}, {112, 52, 20, 26}, {56, 97, 17, 30},
                    {111, 145, 17, 30});
    register_weapon("Arco compuesto", std::string(assets::ARCOCOMPUESTO),
                    {29, 11, 16, 23}, {36, 58, 16, 23}, {7, 107, 16, 24},
                    {5, 153, 16, 24});
    register_weapon("Escudo de tortuga", std::string(assets::ESCUDOTORTUGA),
                    {116, 11, 17, 21}, {31, 55, 17, 21}, {4, 107, 18, 21},
                    {4, 107, 18, 21});
    register_weapon("Escudo de hierro", std::string(assets::ESCUDOHIERRO),
                    {115, 9, 17, 23}, {32, 53, 18, 24}, {3, 105, 18, 23},
                    {3, 105, 18, 23});

    set_adjust("Espada",
               {0, 30, false}, DEFAULT_ADJUST_N,
               {10, 16, true}, {-10, 16, true});
    set_adjust("Hacha",
               {0, 20, false}, DEFAULT_ADJUST_N,
               {10, 18, true}, {-10, 18, true});
    set_adjust("Martillo",
               {-10, 19, false}, DEFAULT_ADJUST_N,
               {10, 16, true}, {-10, 16, true});
    set_adjust("Vara de fresno",
               {0, 10, false}, DEFAULT_ADJUST_N,
               {10, 16, true}, {-10, 16, true});
    set_adjust("Flauta elfica",
               {-10, 15, false}, DEFAULT_ADJUST_N,
               {5, 10, true}, {-5, 10, true});
    set_adjust("Baculo nudoso",
               {-10, 15, false}, DEFAULT_ADJUST_N,
               {5, 10, true}, {-5, 10, true});
    set_adjust("Baculo engarzado",
               {-10, 15, false}, DEFAULT_ADJUST_N,
               {5, 10, true}, {-5, 10, true});
    set_adjust("Arco simple",
               {-10, 18, false}, DEFAULT_ADJUST_N,
               {0, 16, true}, {0, 16, true});
    set_adjust("Arco compuesto",
               {-10, 18, false}, DEFAULT_ADJUST_N,
               {0, 16, true}, {0, 16, true});
    set_adjust("Escudo de tortuga",
               {10, 18, false}, DEFAULT_ADJUST_N,
               {5, 18, true}, {-5, 18, false});
    set_adjust("Escudo de hierro",
               {10, 18, false}, DEFAULT_ADJUST_N,
               {5, 18, true}, {-5, 18, false});
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

void WeaponSpriteRegistry::register_weapon(const std::string& name,
                                           const std::string& sheet,
                                           const SDL_Rect& s, const SDL_Rect& n,
                                           const SDL_Rect& e,
                                           const SDL_Rect& w) {
    if (SDL_Texture* tex = load_texture(sheet)) {
        WeaponSprite ws;
        ws.tex = tex;
        ws.rects[0] = s;
        ws.rects[1] = n;
        ws.rects[2] = e;
        ws.rects[3] = w;
        ws.adjust[0] = DEFAULT_ADJUST_S;
        ws.adjust[1] = DEFAULT_ADJUST_N;
        ws.adjust[2] = DEFAULT_ADJUST_E;
        ws.adjust[3] = DEFAULT_ADJUST_W;
        weapons[name] = ws;
    }
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
