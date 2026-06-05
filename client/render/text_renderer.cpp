#include "render/text_renderer.h"

#include <cstdio>
#include <iostream>

namespace {
int ttf_refcount = 0;

std::string cache_key(const std::string& text, SDL_Color c) {
    char prefix[9];
    std::snprintf(prefix, sizeof(prefix), "%02x%02x%02x%02x", c.r, c.g, c.b,
                  c.a);
    return std::string(prefix) + text;
}
}

TextRenderer::TextRenderer(SDL_Renderer* renderer,
                           const std::filesystem::path& font_path,
                           int pt_size):
    renderer(renderer), font(nullptr) {
    if (ttf_refcount == 0) {
        if (TTF_Init() != 0) {
            std::cerr << "[TextRenderer] TTF_Init fallo: " << TTF_GetError()
                      << "\n";
            return;
        }
    }
    ++ttf_refcount;

    font = TTF_OpenFont(font_path.string().c_str(), pt_size);
    if (!font) {
        std::cerr << "[TextRenderer] No se pudo cargar la fuente: "
                  << font_path << " (" << TTF_GetError() << ")\n";
    }
}

TextRenderer::~TextRenderer() {
    for (auto& [key, entry] : cache) {
        if (entry.tex) {
            SDL_DestroyTexture(entry.tex);
        }
    }
    cache.clear();

    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }

    if (ttf_refcount > 0) {
        --ttf_refcount;
        if (ttf_refcount == 0) {
            TTF_Quit();
        }
    }
}

const TextRenderer::CachedText* TextRenderer::get_or_build(
    const std::string& text, SDL_Color color) {
    if (!font || text.empty()) {
        return nullptr;
    }

    const std::string key = cache_key(text, color);
    auto it = cache.find(key);
    if (it != cache.end()) {
        return &it->second;
    }

    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surface) {
        return nullptr;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);

    CachedText entry;
    if (tex) {
        entry.tex = tex;
        entry.w = surface->w;
        entry.h = surface->h;
    }
    SDL_FreeSurface(surface);

    if (!entry.tex) {
        return nullptr;
    }
    auto result = cache.emplace(key, entry);
    return &result.first->second;
}

void TextRenderer::draw(const std::string& text, int x, int y,
                        SDL_Color color) {
    const CachedText* entry = get_or_build(text, color);
    if (!entry) {
        return;
    }
    SDL_Rect dst{x, y, entry->w, entry->h};
    SDL_RenderCopy(renderer, entry->tex, nullptr, &dst);
}

void TextRenderer::draw_centered(const std::string& text, int center_x, int y,
                                 SDL_Color color) {
    const CachedText* entry = get_or_build(text, color);
    if (!entry) {
        return;
    }
    SDL_Rect dst{center_x - entry->w / 2, y, entry->w, entry->h};
    SDL_RenderCopy(renderer, entry->tex, nullptr, &dst);
}

int TextRenderer::line_height() const {
    return font ? TTF_FontHeight(font) : 0;
}
