#include "render/hud_renderer.h"

#include <filesystem>
#include <string>

HudRenderer::HudRenderer(SDL2pp::Renderer& renderer,
                         const ClientConfig& config):
    renderer(renderer),
    config(config),
    text(renderer.Get(),
         (std::filesystem::current_path() / config.font_path)
             .lexically_normal(),
         config.font_size) {}

void HudRenderer::draw_bar(int x, int y, int w, int h, float ratio,
                           SDL_Color fill) {
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;

    renderer.SetDrawColor(40, 40, 40, 220);
    renderer.FillRect(SDL2pp::Rect(x, y, w, h));

    renderer.SetDrawColor(fill.r, fill.g, fill.b, fill.a);
    renderer.FillRect(SDL2pp::Rect(x, y, static_cast<int>(w * ratio), h));

    renderer.SetDrawColor(0, 0, 0, 255);
    renderer.DrawRect(SDL2pp::Rect(x, y, w, h));
}

void HudRenderer::draw_error_toast(const ClientGameState& state) {
    const uint32_t seq = state.get_error_seq();
    if (seq != last_error_seq) {
        last_error_seq = seq;
        error_shown_at = SDL_GetTicks();
    }
    if (seq == 0 || !state.has_pending_error()) {
        return;
    }
    const Uint32 TOAST_MS = 4000;
    if (SDL_GetTicks() - error_shown_at >= TOAST_MS) {
        return;
    }

    const std::string& msg = state.get_last_error();
    if (msg.empty()) {
        return;
    }
    int tw = 0;
    int th = 0;
    text.size_text(msg, tw, th);

    const int pad = 8;
    const int box_w = tw + pad * 2;
    const int box_h = th + pad * 2;
    const SDL2pp::Point out = renderer.GetOutputSize();
    const int box_x = out.x / 2 - box_w / 2;
    const int box_y = 12;

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(120, 20, 20, 210);
    renderer.FillRect(SDL2pp::Rect(box_x, box_y, box_w, box_h));
    renderer.SetDrawColor(0, 0, 0, 255);
    renderer.DrawRect(SDL2pp::Rect(box_x, box_y, box_w, box_h));
    text.draw_centered(msg, out.x / 2, box_y + pad,
                       SDL_Color{255, 230, 230, 255});
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
}

void HudRenderer::draw_inventory(const ClientGameState& state) {
    const auto& slots = state.get_inventory();
    if (slots.empty()) return;

    const SDL_Color white{255, 255, 255, 255};
    const SDL_Color gray{180, 180, 180, 255};
    const SDL_Color yellow{255, 220, 60, 255};

    const int row_h   = text.line_height() + 4;
    const int panel_w = 240;
    const int pad     = 8;
    const int panel_h = pad * 2 + text.line_height() + 4 +
                        static_cast<int>(slots.size()) * row_h;

    const SDL2pp::Point out = renderer.GetOutputSize();
    const int panel_x = out.x - panel_w - 8;
    const int panel_y = 8;

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 180);
    renderer.FillRect(SDL2pp::Rect(panel_x, panel_y, panel_w, panel_h));
    renderer.SetDrawColor(80, 80, 80, 255);
    renderer.DrawRect(SDL2pp::Rect(panel_x, panel_y, panel_w, panel_h));

    text.draw_centered("INVENTARIO", panel_x + panel_w / 2,
                       panel_y + pad, white);

    int y = panel_y + pad + text.line_height() + 4;

    for (size_t i = 0; i < slots.size(); ++i) {
        const auto& slot = slots[i];
        const int row_y = y + static_cast<int>(i) * row_h;

        // fondo destacado para items equipados
        if (!slot.empty() && slot.equipado) {
            renderer.SetDrawColor(60, 80, 40, 120);
            renderer.FillRect(SDL2pp::Rect(panel_x + 2, row_y,
                                           panel_w - 4, row_h));
        }

        std::string line = "[" + std::to_string(i) + "] ";
        if (slot.empty()) {
            text.draw(line + "(vacio)", panel_x + pad, row_y, gray);
        } else {
            line += slot.item;
            if (slot.cantidad > 1)
                line += " x" + std::to_string(slot.cantidad);
            if (slot.equipado)
                line += " [E]";
            text.draw(line, panel_x + pad, row_y,
                      slot.equipado ? yellow : white);
        }
    }

    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
}

void HudRenderer::render(const ClientGameState& state) {
    draw_error_toast(state);
    if (state.is_inventory_open()) draw_inventory(state);
    
    if (!state.has_local_stats()) {
        return;
    }
    const PlayerStats& s = state.get_local_stats();

    const int x = 8;
    const int y = 8;
    const int bar_w = 190;
    const int bar_h = 18;
    const int gap = 6;
    const SDL_Color white{255, 255, 255, 255};

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);

    // panel de fondo semitransparente
    const int panel_h = bar_h * 2 + gap * 3 + text.line_height();
    renderer.SetDrawColor(0, 0, 0, 150);
    renderer.FillRect(SDL2pp::Rect(x - 4, y - 4, bar_w + 8, panel_h));

    // vida
    const float vida_ratio =
        s.vida_max > 0 ? static_cast<float>(s.vida) / s.vida_max : 0.0f;
    draw_bar(x, y, bar_w, bar_h, vida_ratio, SDL_Color{200, 40, 40, 255});
    text.draw("Vida " + std::to_string(s.vida) + "/" +
                  std::to_string(s.vida_max),
              x + 4, y - 1, white);

    // mana
    const int y2 = y + bar_h + gap;
    const float mana_ratio =
        s.mana_max > 0 ? static_cast<float>(s.mana) / s.mana_max : 0.0f;
    draw_bar(x, y2, bar_w, bar_h, mana_ratio, SDL_Color{40, 90, 220, 255});
    text.draw("Mana " + std::to_string(s.mana) + "/" +
                  std::to_string(s.mana_max),
              x + 4, y2 - 1, white);

    // nivel / exp / oro
    const int y3 = y2 + bar_h + gap;
    text.draw("Nivel " + std::to_string(s.nivel) + "   Exp " +
                  std::to_string(s.experiencia) + "   Oro " +
                  std::to_string(s.oro),
              x, y3, white);

    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
}