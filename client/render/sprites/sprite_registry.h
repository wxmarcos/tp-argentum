#ifndef CLIENT_RENDER_SPRITE_REGISTRY_H
#define CLIENT_RENDER_SPRITE_REGISTRY_H

#include <SDL2/SDL.h>

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>

#include "render/sprites/sprite_data.h"
#include "render/texture_manager.h"

class SpriteRegistry {
private:
    TextureManager textures;
    std::unordered_map<std::string, SpriteData> sprite_data;
    std::unordered_map<std::string, HeadData> head_data;

    void register_sprite(std::string_view key, std::string_view relative_path,
                         const SpriteData& data);

    void register_head(std::string_view raza, std::string_view relative_path,
                       const HeadData& data);

    void load_all();

    void load_player_bodies();

    void load_class_bodies();

    void load_creatures();

    void load_heads();

    void load_helmets();

    void load_armor_bodies();

public:
    SpriteRegistry(SDL_Renderer* renderer,
                   const std::filesystem::path& assets_root);

    SDL_Texture* get_texture(const std::string& key) const;

    SDL_Rect get_frame(const std::string& key, int dir_idx, int frame) const;

    int get_head_neck(const std::string& key) const;

    bool has(const std::string& key) const;

    SDL_Texture* get_head_texture(const std::string& raza) const;

    SDL_Rect get_head_rect(const std::string& raza, int dir_idx) const;

    bool has_head(const std::string& raza) const;

    SDL_Texture* get_helmet_texture(const std::string& casco_key) const;

    SDL_Rect get_helmet_rect(const std::string& casco_key, int dir_idx) const;

    bool has_helmet(const std::string& casco_key) const;

    int get_helmet_off_x(const std::string& helmet_key, int dir_idx) const;

    int get_helmet_off_y(const std::string& helmet_key, int dir_idx) const;

    int get_helmet_scale(const std::string& helmet_key) const;
};

#endif
