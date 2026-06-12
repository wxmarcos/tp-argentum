#include "render/sprites/sprite_registry.h"

#include <iostream>
#include <string>
#include <string_view>

#include "game/entity_keys.h"

// Cuerpos por raza
static constexpr std::string_view BODY_HUMANO =
    "cuerpos/razas/humanoCuerpo.png";
static constexpr std::string_view BODY_FANTASMA = "cuerpos/fantasmin.png";
static constexpr std::string_view BODY_ELFO = "cuerpos/razas/elfoCuerpo.png";
static constexpr std::string_view BODY_ENANO = "cuerpos/razas/enanoCuerpo.png";
static constexpr std::string_view BODY_GNOMO = "cuerpos/razas/gnomoCuerpo.png";

// Cuerpos por clase
static constexpr std::string_view BODY_MAGO = "cuerpos/clases/magoCuerpo.png";
static constexpr std::string_view BODY_CLERIGO =
    "cuerpos/clases/clerigoCuerpo.png";
static constexpr std::string_view BODY_GUERRERO =
    "cuerpos/clases/guerreroCuerpo.png";
static constexpr std::string_view BODY_PALADIN =
    "cuerpos/clases/paladinCuerpo.png";

// Criaturas
static constexpr std::string_view CRE_GOBLIN = "criaturas/goblin.png";
static constexpr std::string_view CRE_ESQUELETO = "criaturas/esqueleto.png";
static constexpr std::string_view CRE_ZOMBIE = "criaturas/zombie.png";
static constexpr std::string_view CRE_ORCO = "criaturas/orco.png";
static constexpr std::string_view CRE_ARANA = "criaturas/arana.png";
static constexpr std::string_view CRE_GOLEM = "criaturas/golem.png";

// NPCs
static constexpr std::string_view NPC_BANQUERO = "characters/banquero.png";
static constexpr std::string_view NPC_COMERCIANTE =
    "characters/comerciante.png";
static constexpr std::string_view NPC_SACERDOTE = "characters/sacerdote.png";

// Cabezas
static constexpr std::string_view HEAD_HUMANO = "cabezas/humanoCabezas.png";
static constexpr std::string_view HEAD_ELFO = "cabezas/elfoCabezas.png";
static constexpr std::string_view HEAD_ENANO = "cabezas/enanoCabezas.png";
static constexpr std::string_view HEAD_GNOMO = "cabezas/gnomoCabezas.png";


SpriteRegistry::SpriteRegistry(SDL_Renderer* renderer,
                               const std::filesystem::path& assets_root):
        textures(renderer, assets_root) {
    load_all();
}

void SpriteRegistry::register_sprite(std::string_view key,
                                     std::string_view relative_path,
                                     const SpriteData& data) {
    const std::string k(key);
    const std::string path(relative_path);
    if (textures.try_load(k, path)) {
        sprite_data[k] = data;
    } else {
        std::cerr << "[SpriteRegistry] No se pudo cargar: " << path << "\n";
    }
}

void SpriteRegistry::register_head(std::string_view raza,
                                   std::string_view relative_path,
                                   const HeadData& data) {
    const std::string raza_key(raza);
    const std::string key = "head_" + raza_key;
    const std::string path(relative_path);
    if (textures.try_load(key, path)) {
        head_data[raza_key] = data;
    } else {
        std::cerr << "[SpriteRegistry] No se pudo cargar cabeza: " << path
                  << "\n";
    }
}

void SpriteRegistry::load_player_bodies() {
    register_sprite(keys::HUMANO, BODY_HUMANO, make_humano_sprite_data());
    register_sprite(keys::FANTASMA, BODY_FANTASMA,
                    make_fantasma_sprite_data());
    register_sprite(keys::ELFO, BODY_ELFO, make_elfo_sprite_data());
    register_sprite(keys::ENANO, BODY_ENANO, make_enano_sprite_data());
    register_sprite(keys::GNOMO, BODY_GNOMO, make_gnomo_sprite_data());
}

void SpriteRegistry::load_class_bodies() {
    register_sprite(keys::MAGO, BODY_MAGO, make_mago_sprite_data());
    register_sprite(keys::CLERIGO, BODY_CLERIGO, make_clerigo_sprite_data());
    register_sprite(keys::GUERRERO, BODY_GUERRERO,
                    make_guerrero_sprite_data());
    register_sprite(keys::PALADIN, BODY_PALADIN, make_paladin_sprite_data());
}

void SpriteRegistry::load_creatures() {
    register_sprite(keys::GOBLIN, CRE_GOBLIN, make_goblin_sprite_data());
    register_sprite(keys::ESQUELETO, CRE_ESQUELETO,
                    make_esqueleto_sprite_data());
    register_sprite(keys::ZOMBIE, CRE_ZOMBIE, make_zombie_sprite_data());
    register_sprite(keys::ORCO, CRE_ORCO, make_orco_sprite_data());
    register_sprite(keys::ARANA, CRE_ARANA, make_arana_sprite_data());
    register_sprite(keys::GOLEM, CRE_GOLEM, make_golem_sprite_data());
    register_sprite(keys::BANQUERO, NPC_BANQUERO, make_banquero_sprite_data());
    register_sprite(keys::COMERCIANTE, NPC_COMERCIANTE,
                    make_comerciante_sprite_data());
    register_sprite(keys::SACERDOTE, NPC_SACERDOTE,
                    make_sacerdote_sprite_data());
}

void SpriteRegistry::load_heads() {
    register_head(keys::HUMANO, HEAD_HUMANO, make_humano_head_data());
    register_head(keys::ELFO, HEAD_ELFO, make_elfo_head_data());
    register_head(keys::ENANO, HEAD_ENANO, make_enano_head_data());
    register_head(keys::GNOMO, HEAD_GNOMO, make_gnomo_head_data());
}

void SpriteRegistry::load_all() {
    load_player_bodies();
    load_class_bodies();
    load_creatures();
    load_heads();
}

SDL_Texture* SpriteRegistry::get_texture(const std::string& key) const {
    return textures.get_or_null(key);
}

SDL_Rect SpriteRegistry::get_frame(const std::string& key, int dir_idx,
                                   int frame) const {
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
