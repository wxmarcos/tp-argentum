#ifndef CLIENT_RENDER_ITEM_SPRITE_REGISTRY_H
#define CLIENT_RENDER_ITEM_SPRITE_REGISTRY_H

#include <string>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#include "config/client_config.h"

struct ItemSprite {
    SDL_Texture* tex = nullptr;
    SDL_Rect src{};
};

class ItemSpriteRegistry {
    private:
    SDL2pp::Renderer& renderer;
    const ClientConfig& config;

    std::unordered_map<std::string, ItemSprite> sprites;
    std::unordered_map<std::string, SDL_Texture*> tex_by_path;
    std::vector<SDL_Texture*> owned;

    void load_defs();

    SDL_Texture* load_texture(const std::string& rel_path);
    
    void register_item(const std::string& name, const std::string& sheet,
                       const SDL_Rect& src);

    public:
    ItemSpriteRegistry(SDL2pp::Renderer& renderer, const ClientConfig& config);

    ~ItemSpriteRegistry();

    const ItemSprite* find(const std::string& name) const;

    ItemSpriteRegistry(const ItemSpriteRegistry&) = delete;
    ItemSpriteRegistry& operator=(const ItemSpriteRegistry&) = delete;
};

#endif