#ifndef CLIENT_RENDER_SPRITES_WEAPON_SPRITE_REGISTRY_H
#define CLIENT_RENDER_SPRITES_WEAPON_SPRITE_REGISTRY_H

#include <string>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#include "config/client_config.h"

struct WeaponSprite {
    SDL_Texture* tex = nullptr;
    SDL_Rect rects[4]{};
};

class WeaponSpriteRegistry {
    private:
    SDL2pp::Renderer& renderer;
    const ClientConfig& config;

    std::unordered_map<std::string, WeaponSprite> weapons;
    std::unordered_map<std::string, SDL_Texture*> tex_by_path;
    std::vector<SDL_Texture*> owned;

    void load_defs();
    SDL_Texture* load_texture(const std::string& rel_path);
    void register_weapon(const std::string& name, const std::string& sheet,
                         const SDL_Rect& s, const SDL_Rect& n,
                         const SDL_Rect& e, const SDL_Rect& w);

    public:
    WeaponSpriteRegistry(SDL2pp::Renderer& renderer,
                         const ClientConfig& config);
    ~WeaponSpriteRegistry();

    const WeaponSprite* find(const std::string& name) const;

    WeaponSpriteRegistry(const WeaponSpriteRegistry&) = delete;
    WeaponSpriteRegistry& operator=(const WeaponSpriteRegistry&) = delete;
};

#endif