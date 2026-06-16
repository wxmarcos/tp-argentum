#include "render/sprites/item_sprite_registry.h"

#include <filesystem>

#include <SDL2/SDL_image.h>

const SDL_Rect arma_icono{0, 224, 32, 32};

ItemSpriteRegistry::ItemSpriteRegistry(SDL2pp::Renderer& renderer,
                                       const ClientConfig& config):
        renderer(renderer), config(config) {
    load_defs();
}

ItemSpriteRegistry::~ItemSpriteRegistry() {
    for (SDL_Texture* t : owned) {
        if (t) {
            SDL_DestroyTexture(t);
        }
    }
}

void ItemSpriteRegistry::load_defs() {
    register_item("Oro", "objetos/otros/oro.png", {64, 0, 32, 32});
    register_item("Pocion de vida", "objetos/otros/pociones.png",
                  {448, 64, 32, 32});
    register_item("Pocion de mana", "objetos/otros/pociones.png",
                  {480, 64, 32, 32});

    register_item("Espada", "objetos/armas/espada.png", arma_icono);
    register_item("Hacha", "objetos/armas/hacha.png", arma_icono);
    register_item("Martillo", "objetos/armas/martillo.png", arma_icono);
    register_item("Vara de fresno", "objetos/armas/varaDeFresno.png",
                  arma_icono);
    register_item("Flauta elfica", "objetos/armas/flautaElfica.png",
                  arma_icono);
    register_item("Baculo nudoso", "objetos/armas/baculoNudoso.png",
                  arma_icono);
    register_item("Baculo engarzado", "objetos/armas/baculoEngarzado.png",
                  arma_icono);
    register_item("Arco simple", "objetos/armas/arcoSimple.png", arma_icono);
    register_item("Arco compuesto", "objetos/armas/arcoCompuesto.png",
                  arma_icono);
    register_item("Escudo de tortuga", "objetos/armas/escudoTortuga.png",
                  arma_icono);
    register_item("Escudo de hierro", "objetos/armas/escudoHierro.png",
                  arma_icono);
}

SDL_Texture* ItemSpriteRegistry::load_texture(const std::string& rel_path) {
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

void ItemSpriteRegistry::register_item(const std::string& name,
                                       const std::string& sheet,
                                       const SDL_Rect& src) {
    if (SDL_Texture* tex = load_texture(sheet)) {
        sprites[name] = {tex, src};
    }
}

const ItemSprite* ItemSpriteRegistry::find(const std::string& name) const {
    auto it = sprites.find(name);
    return (it != sprites.end()) ? &it->second : nullptr;
}
