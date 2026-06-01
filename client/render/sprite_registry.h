#ifndef CLIENT_RENDER_SPRITE_REGISTRY_H
#define CLIENT_RENDER_SPRITE_REGISTRY_H
 
#include <filesystem>
#include <string>
#include <unordered_map>
 
#include <SDL2/SDL.h>
 
#include "render/sprite_data.h"
#include "render/texture_manager.h"
 
class SpriteRegistry {
    private:
    TextureManager textures;
    std::unordered_map<std::string, SpriteData> sprite_data;
    std::unordered_map<std::string, HeadData> head_data;
 
    void register_sprite(const std::string& key,
                         const std::string& relative_path,
                         const SpriteData& data);
 
    void register_head(const std::string& raza,
                       const std::string& relative_path,
                       const HeadData& data);
 
    void load_all();
 
    public:
    SpriteRegistry(SDL_Renderer* renderer, const std::filesystem::path& assets_root);
 
    SDL_Texture* get_texture(const std::string& key) const;
 
    SDL_Rect get_frame(const std::string& key, int dir_idx, int frame) const;
 
    int get_head_neck(const std::string& key) const;
 
    bool has(const std::string& key) const;
 
    SDL_Texture* get_head_texture(const std::string& raza) const;
 
    SDL_Rect get_head_rect(const std::string& raza, int dir_idx) const;
 
    bool has_head(const std::string& raza) const;
};
 
#endif