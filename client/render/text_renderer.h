#ifndef CLIENT_RENDER_TEXT_RENDERER_H
#define CLIENT_RENDER_TEXT_RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <filesystem>
#include <string>
#include <unordered_map>

class TextRenderer {
public:
    TextRenderer(SDL_Renderer* renderer,
                 const std::filesystem::path& font_path, int pt_size);
    ~TextRenderer();

    TextRenderer(const TextRenderer&) = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;

    bool ok() const { return font != nullptr; }

    void draw(const std::string& text, int x, int y, SDL_Color color);
    void draw_centered(const std::string& text, int center_x, int y,
                       SDL_Color color);

    int line_height() const;

private:
    struct CachedText {
        SDL_Texture* tex = nullptr;
        int w = 0;
        int h = 0;
    };

    SDL_Renderer* renderer;
    TTF_Font* font;
    std::unordered_map<std::string, CachedText> cache;

    const CachedText* get_or_build(const std::string& text, SDL_Color color);
};

#endif