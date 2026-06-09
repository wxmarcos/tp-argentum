#ifndef CLIENT_RENDER_TEXTURE_MANAGER_H
#define CLIENT_RENDER_TEXTURE_MANAGER_H

#include <SDL2/SDL.h>

#include <filesystem>
#include <string>
#include <unordered_map>

class TextureManager {
private:
    SDL_Renderer* renderer;
    std::filesystem::path assets_root;
    std::unordered_map<std::string, SDL_Texture*> cache;

    SDL_Texture* load_png(const std::filesystem::path& full_path);

public:
    TextureManager(SDL_Renderer* renderer, std::filesystem::path assets_root);
    ~TextureManager();

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    void load(const std::string& key, const std::string& relative_path);

    bool try_load(const std::string& key, const std::string& relative_path);

    SDL_Texture* get(const std::string& key) const;

    SDL_Texture* get_or_null(const std::string& key) const;

    bool has(const std::string& key) const;
};

#endif
