#include "render/sprites/sprite_registry.h"

#include <iostream>
#include <string>
#include <string_view>

#include "game/entity_keys.h"
#include "render/asset_paths.h"

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
    register_sprite(keys::HUMANO, assets::BODY_HUMANO,
                    make_humano_sprite_data());
    register_sprite(keys::FANTASMA, assets::BODY_FANTASMA,
                    make_fantasma_sprite_data());
    register_sprite(keys::ELFO, assets::BODY_ELFO, make_elfo_sprite_data());
    register_sprite(keys::ENANO, assets::BODY_ENANO, make_enano_sprite_data());
    register_sprite(keys::GNOMO, assets::BODY_GNOMO, make_gnomo_sprite_data());
}

void SpriteRegistry::load_class_bodies() {
    register_sprite(keys::MAGO, assets::BODY_MAGO, make_mago_sprite_data());
    register_sprite(keys::CLERIGO, assets::BODY_CLERIGO,
                    make_clerigo_sprite_data());
    register_sprite(keys::GUERRERO, assets::BODY_GUERRERO,
                    make_guerrero_sprite_data());
    register_sprite(keys::PALADIN, assets::BODY_PALADIN,
                    make_paladin_sprite_data());
}

void SpriteRegistry::load_creatures() {
    register_sprite(keys::GOBLIN, assets::CRE_GOBLIN,
                    make_goblin_sprite_data());
    register_sprite(keys::ESQUELETO, assets::CRE_ESQUELETO,
                    make_esqueleto_sprite_data());
    register_sprite(keys::ZOMBIE, assets::CRE_ZOMBIE,
                    make_zombie_sprite_data());
    register_sprite(keys::ORCO, assets::CRE_ORCO, make_orco_sprite_data());
    register_sprite(keys::ARANA, assets::CRE_ARANA, make_arana_sprite_data());
    register_sprite(keys::GOLEM, assets::CRE_GOLEM, make_golem_sprite_data());
    register_sprite(keys::ESQUELETO_HACHA, assets::CRE_ESQUELETO_HACHA,
                    make_esqueleto_hacha_sprite_data());
    register_sprite(keys::ARANA_BLANCA, assets::CRE_ARANA_BLANCA,
                    make_arana_blanca_sprite_data());
    register_sprite(keys::GOBLIN_JOROBADO, assets::CRE_GOBLIN_JOROBADO,
                    make_goblin_jorobado_sprite_data());
    register_sprite(keys::GOLEM_DEMONIACO, assets::CRE_GOLEM_DEMONIACO,
                    make_golem_demoniaco_sprite_data());
    register_sprite(keys::BANQUERO, assets::NPC_BANQUERO,
                    make_banquero_sprite_data());
    register_sprite(keys::COMERCIANTE, assets::NPC_COMERCIANTE,
                    make_comerciante_sprite_data());
    register_sprite(keys::SACERDOTE, assets::NPC_SACERDOTE,
                    make_sacerdote_sprite_data());
}

void SpriteRegistry::load_heads() {
    register_head(keys::HUMANO, assets::HEAD_HUMANO, make_humano_head_data());
    register_head(keys::ELFO, assets::HEAD_ELFO, make_elfo_head_data());
    register_head(keys::ENANO, assets::HEAD_ENANO, make_enano_head_data());
    register_head(keys::GNOMO, assets::HEAD_GNOMO, make_gnomo_head_data());
}

void SpriteRegistry::load_helmets() {
    register_head(keys::CASCO_CAPUCHA, assets::CASCO_CAPUCHA,
                  make_casco_capucha_data());
    register_head(keys::CASCO_HIERRO, assets::CASCO_HIERRO,
                  make_casco_hierro_data());
    register_head(keys::CASCO_SOMBRERO, assets::CASCO_SOMBRERO_MAGICO,
                  make_casco_sombrero_data());
}

void SpriteRegistry::load_armor_bodies() {
    register_sprite(keys::ARMADURA_CUERO, assets::ARMADURA_CUERO,
                    make_armadura_cuero_sprite_data());
    register_sprite(keys::ARMADURA_PLACAS, assets::ARMADURA_PLACAS,
                    make_armadura_placas_sprite_data());
    register_sprite(keys::TUNICA_AZUL, assets::TUNICA_AZUL,
                    make_tunica_azul_sprite_data());
}

void SpriteRegistry::load_all() {
    load_player_bodies();
    load_class_bodies();
    load_armor_bodies();
    load_creatures();
    load_heads();
    load_helmets();
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

SDL_Texture* SpriteRegistry::get_helmet_texture(
    const std::string& helmet_key) const {
    return textures.get_or_null("head_" + helmet_key);
}

SDL_Rect SpriteRegistry::get_helmet_rect(const std::string& helmet_key,
                                         int dir_idx) const {
    auto it = head_data.find(helmet_key);
    if (it == head_data.end()) return SDL_Rect{0, 0, 8, 8};
    return it->second.head_rects[dir_idx % 4];
}

bool SpriteRegistry::has_helmet(const std::string& helmet_key) const {
    return head_data.count(helmet_key) > 0;
}

int SpriteRegistry::get_helmet_off_x(const std::string& helmet_key,
                                     int dir_idx) const {
    auto it = head_data.find(helmet_key);
    if (it == head_data.end()) return 0;
    return it->second.off_x[dir_idx % 4];
}

int SpriteRegistry::get_helmet_off_y(const std::string& helmet_key,
                                     int dir_idx) const {
    auto it = head_data.find(helmet_key);
    if (it == head_data.end()) return 0;
    return it->second.off_y[dir_idx % 4];
}

int SpriteRegistry::get_helmet_scale(const std::string& helmet_key) const {
    auto it = head_data.find(helmet_key);
    if (it == head_data.end()) return 100;
    return it->second.scale_pct;
}
