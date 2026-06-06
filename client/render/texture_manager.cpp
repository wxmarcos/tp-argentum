#include "render/texture_manager.h"

#include <SDL_image.h>

#include <iostream>
#include <stdexcept>

TextureManager::TextureManager(SDL_Renderer* renderer,
                               std::filesystem::path assets_root):
    renderer(renderer),
    assets_root(std::move(assets_root)) {}

TextureManager::~TextureManager() {
    for (auto& [key, tex] : cache) {
        SDL_DestroyTexture(tex);
    }
}

SDL_Texture* TextureManager::load_png(const std::filesystem::path& full_path) {
    SDL_Surface* surface = IMG_Load(full_path.c_str());
    if (!surface) {
        return nullptr;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return tex;
}

void TextureManager::load(const std::string& key,
                          const std::string& relative_path) {
    if (cache.count(key)) {
        return;
    }
    std::filesystem::path full = assets_root / relative_path;
    SDL_Texture* tex = load_png(full);
    if (!tex) {
        throw std::runtime_error("[TextureManager] No se pudo cargar: " +
                                 full.string() + " — " + IMG_GetError());
    }
    cache[key] = tex;
}

bool TextureManager::try_load(const std::string& key,
                              const std::string& relative_path) {
    if (cache.count(key)) {
        return true;
    }
    std::filesystem::path full = assets_root / relative_path;
    SDL_Texture* tex = load_png(full);
    if (!tex) {
        std::cerr << "[TextureManager] Advertencia: no se pudo cargar: " << full
                  << " — " << IMG_GetError() << "\n";
        return false;
    }
    cache[key] = tex;
    return true;
}

SDL_Texture* TextureManager::get(const std::string& key) const {
    auto it = cache.find(key);
    if (it == cache.end()) {
        throw std::runtime_error("[TextureManager] Clave no cargada: " + key);
    }
    return it->second;
}

SDL_Texture* TextureManager::get_or_null(const std::string& key) const {
    auto it = cache.find(key);
    return (it != cache.end()) ? it->second : nullptr;
}

bool TextureManager::has(const std::string& key) const {
    return cache.count(key) > 0;
}
