
#include "render/sprite_registry.h"
#include <iostream>
 
SpriteRegistry::SpriteRegistry(SDL_Renderer* renderer,
                               const std::filesystem::path& assets_root):
        textures(renderer, assets_root) {
    load_all();
}
 
void SpriteRegistry::register_sprite(const std::string& key,
                                     const std::string& relative_path,
                                     const SpriteData& data) {
    bool loaded = textures.try_load(key, relative_path);
    if (loaded) {
        sprite_data[key] = data;
    } else {
        std::cerr << "[SpriteRegistry] No se pudo cargar: "
                  << relative_path << "\n";
    }
}
 
void SpriteRegistry::register_head(const std::string& raza,
                                   const std::string& relative_path,
                                   const HeadData& data) {
    const std::string key = "head_" + raza;
    bool loaded = textures.try_load(key, relative_path);
    if (loaded) {
        head_data[raza] = data;
    } else {
        std::cerr << "[SpriteRegistry] No se pudo cargar cabeza: "
                  << relative_path << "\n";
    }
}
 
void SpriteRegistry::load_all() {
    const SpriteData std_data = make_standard_sprite_data();
 
    register_sprite("humano",  "cuerpos/razas/humanoCuerpo.png",
                    make_humano_sprite_data());
    register_sprite("elfo",    "cuerpos/razas/elfoCuerpo.png",
                    make_elfo_sprite_data());
    register_sprite("enano",   "cuerpos/razas/enanoCuerpo.png",
                    make_enano_sprite_data());
    register_sprite("gnomo",   "cuerpos/razas/gnomoCuerpo.png",
                    make_gnomo_sprite_data());
 
    register_sprite("mago",    "cuerpos/clases/magoCuerpo.png",
                    make_mago_sprite_data());
    register_sprite("clerigo", "cuerpos/clases/clerigoCuerpo.png",
                    make_clerigo_sprite_data());
    register_sprite("guerrero","cuerpos/clases/guerreroCuerpo.png",
                    make_guerrero_sprite_data());
    register_sprite("paladin", "cuerpos/clases/paladinCuerpo.png",
                    make_paladin_sprite_data());
 
    register_sprite("goblin",   "criaturas/goblin.png",   std_data);
    register_sprite("esqueleto","criaturas/esqueleto.png",std_data);
    register_sprite("zombie",   "criaturas/zombie.png",   std_data);
    register_sprite("orco",     "criaturas/orco.png",     std_data);
    register_sprite("arana",    "criaturas/arana.png",    std_data);
    register_sprite("golem",    "criaturas/golem.png",    std_data);
 
    register_sprite("banquero",    "characters/banquero.png",    std_data);
    register_sprite("comerciante", "characters/comerciante.png", std_data);
    register_sprite("sacerdote",   "characters/sacerdote.png",   std_data);
 
    register_head("humano", "cabezas/humanoCabezas.png", make_humano_head_data());
    register_head("elfo",   "cabezas/elfoCabezas.png",   make_elfo_head_data());
    register_head("enano",  "cabezas/enanoCabezas.png",  make_enano_head_data());
    register_head("gnomo",  "cabezas/gnomoCabezas.png",  make_gnomo_head_data());
 
    textures.try_load("tile_grass", "5144.png");
}
 
SDL_Texture* SpriteRegistry::get_texture(const std::string& key) const {
    return textures.get_or_null(key);
}
 
SDL_Rect SpriteRegistry::get_frame(const std::string& key,
                                   int dir_idx, int frame) const {
    auto it = sprite_data.find(key);
    if (it == sprite_data.end()) return SDL_Rect{0, 0, 16, 16};
    const int f = frame % WALK_FRAME_COUNT;
    const int d = dir_idx % 4;
    return it->second.frame_rects[d][f];
}
 
int SpriteRegistry::get_head_neck(const std::string& key) const {
    auto it = sprite_data.find(key);
    if (it == sprite_data.end()) {
        return DEFAULT_HUMAN_HEAD_NECK;
    }
    return it->second.head_neck;
}
 
bool SpriteRegistry::has(const std::string& key) const {
    return sprite_data.count(key) > 0;
}
 
SDL_Texture* SpriteRegistry::get_head_texture(const std::string& raza) const {
    return textures.get_or_null("head_" + raza);
}
 
SDL_Rect SpriteRegistry::get_head_rect(const std::string& raza,
                                       int dir_idx) const {
    auto it = head_data.find(raza);
    if (it == head_data.end()) return SDL_Rect{0, 0, 8, 8};
    return it->second.head_rects[dir_idx % 4];
}
 
bool SpriteRegistry::has_head(const std::string& raza) const {
    return head_data.count(raza) > 0;
}
 