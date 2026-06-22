#ifndef CLIENT_RENDER_TEXT_RENDERER_H
#define CLIENT_RENDER_TEXT_RENDERER_H

#include <SDL2/SDL.h>
#include <SDL_ttf.h>

#include <filesystem>
#include <string>
#include <unordered_map>

struct CachedText {
    SDL_Texture* tex = nullptr;
    int w = 0;
    int h = 0;
};

class TextRenderer {
private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    std::unordered_map<std::string, CachedText> cache;

    static int ttf_refcount;

    static std::string cache_key(const std::string& text, SDL_Color color);

    const CachedText* get_or_build(const std::string& text, SDL_Color color);

public:
    TextRenderer(SDL_Renderer* renderer, const std::filesystem::path& font_path,
                 int pt_size);

    ~TextRenderer();

    TextRenderer(const TextRenderer&) = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;

    bool ok() const;

    void draw(const std::string& text, int x, int y, SDL_Color color);

    void draw_centered(const std::string& text, int center_x, int y,
                       SDL_Color color, Uint8 alpha = 255);

    int line_height() const;

    void size_text(const std::string& text, int& w, int& h) const;
};

#endif
