#include "render/hud_renderer.h"

#include <SDL2/SDL_image.h>

#include <cstddef>
#include <filesystem>
#include <string>

#include "render/asset_paths.h"
#include "render/colors.h"

static constexpr Uint32 TOAST_MS = 4000;
static constexpr int TOAST_PAD = 8;
static constexpr int TOAST_MARGIN_TOP = 12;

static constexpr int INV_PANEL_W = 240;
static constexpr int INV_PAD = 8;
static constexpr int INV_MARGIN = 8;
static constexpr int ROW_EXTRA = 4;

static constexpr int HUD_MARGIN = 8;
static constexpr int BAR_W = 190;
static constexpr int BAR_H = 18;
static constexpr int BAR_GAP = 6;
static constexpr int PANEL_PAD = 4;

static constexpr SDL_Color BAR_TRACK{40, 40, 40, 220};
static constexpr SDL_Color PANEL_BG{0, 0, 0, 180};
static constexpr SDL_Color PANEL_BORDER{80, 80, 80, 255};
static constexpr SDL_Color EQUIPPED_ROW{60, 80, 40, 120};
static constexpr SDL_Color STATS_PANEL_BG{0, 0, 0, 150};

HudRenderer::HudRenderer(SDL2pp::Renderer& renderer,
                         const ClientConfig& config):
        renderer(renderer),
        config(config),
        text(renderer.Get(),
             (std::filesystem::current_path() / config.font_path)
                 .lexically_normal(),
             config.font_size),
        tex_frame(nullptr) {
    const auto path = (std::filesystem::current_path() / config.assets_path /
                       assets::UI_MARCO)
                          .lexically_normal();
    SDL_Surface* s = IMG_Load(path.string().c_str());
    if (s) {
        tex_frame = SDL_CreateTextureFromSurface(renderer.Get(), s);
        SDL_FreeSurface(s);
    }
}

void HudRenderer::draw_bar(int x, int y, int w, int h, float ratio,
                           SDL_Color fill) {
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;

    renderer.SetDrawColor(BAR_TRACK.r, BAR_TRACK.g, BAR_TRACK.b, BAR_TRACK.a);
    renderer.FillRect(SDL2pp::Rect(x, y, w, h));

    renderer.SetDrawColor(fill.r, fill.g, fill.b, fill.a);
    renderer.FillRect(SDL2pp::Rect(x, y, static_cast<int>(w * ratio), h));

    renderer.SetDrawColor(colors::BLACK.r, colors::BLACK.g, colors::BLACK.b,
                          colors::BLACK.a);
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

    const int box_w = tw + TOAST_PAD * 2;
    const int box_h = th + TOAST_PAD * 2;
    const SDL2pp::Point out = renderer.GetOutputSize();
    const int box_x = out.x / 2 - box_w / 2;
    const int box_y = TOAST_MARGIN_TOP;

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(colors::ERROR_TOAST_BG.r, colors::ERROR_TOAST_BG.g,
                          colors::ERROR_TOAST_BG.b, colors::ERROR_TOAST_BG.a);
    renderer.FillRect(SDL2pp::Rect(box_x, box_y, box_w, box_h));
    renderer.SetDrawColor(colors::BLACK.r, colors::BLACK.g, colors::BLACK.b,
                          colors::BLACK.a);
    renderer.DrawRect(SDL2pp::Rect(box_x, box_y, box_w, box_h));
    text.draw_centered(msg, out.x / 2, box_y + TOAST_PAD, colors::ERROR_TEXT);
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
}

void HudRenderer::draw_inventory_panel(int panel_x, int panel_y, int panel_h) {
    renderer.SetDrawColor(PANEL_BG.r, PANEL_BG.g, PANEL_BG.b, PANEL_BG.a);
    renderer.FillRect(SDL2pp::Rect(panel_x, panel_y, INV_PANEL_W, panel_h));
    renderer.SetDrawColor(PANEL_BORDER.r, PANEL_BORDER.g, PANEL_BORDER.b,
                          PANEL_BORDER.a);
    renderer.DrawRect(SDL2pp::Rect(panel_x, panel_y, INV_PANEL_W, panel_h));
    text.draw_centered("INVENTARIO", panel_x + INV_PANEL_W / 2,
                       panel_y + INV_PAD, colors::WHITE);
}

void HudRenderer::draw_inventory_slot(const InventorySlotView& slot,
                                      size_t idx, int panel_x, int row_y,
                                      int row_h) {
    if (!slot.empty() && slot.equipado) {
        renderer.SetDrawColor(EQUIPPED_ROW.r, EQUIPPED_ROW.g, EQUIPPED_ROW.b,
                              EQUIPPED_ROW.a);
        renderer.FillRect(
            SDL2pp::Rect(panel_x + 2, row_y, INV_PANEL_W - 4, row_h));
    }

    std::string line = "[" + std::to_string(idx) + "] ";
    if (slot.empty()) {
        text.draw(line + "(vacio)", panel_x + INV_PAD, row_y,
                  colors::TEXT_GRAY);
        return;
    }
    line += slot.item;
    if (slot.cantidad > 1) line += " x" + std::to_string(slot.cantidad);
    if (slot.equipado) line += " [E]";
    text.draw(line, panel_x + INV_PAD, row_y,
              slot.equipado ? colors::ITEM_EQUIPPED : colors::WHITE);
}

void HudRenderer::draw_inventory(const ClientGameState& state) {
    const auto& slots = state.get_inventory();
    if (slots.empty()) return;

    const int row_h = text.line_height() + ROW_EXTRA;
    const int panel_h = INV_PAD * 2 + text.line_height() + ROW_EXTRA +
                        static_cast<int>(slots.size()) * row_h;
    const SDL2pp::Point out = renderer.GetOutputSize();
    const int panel_x = out.x - INV_PANEL_W - INV_MARGIN;
    const int panel_y = INV_MARGIN;

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    draw_inventory_panel(panel_x, panel_y, panel_h);

    const int first_y = panel_y + INV_PAD + text.line_height() + ROW_EXTRA;
    for (size_t i = 0; i < slots.size(); ++i) {
        const int row_y = first_y + static_cast<int>(i) * row_h;
        draw_inventory_slot(slots[i], i, panel_x, row_y, row_h);
    }
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
}

void HudRenderer::draw_stats(const PlayerStats& s) {
    const int x = HUD_MARGIN;
    const int y = HUD_MARGIN;
    const int panel_h = BAR_H * 2 + BAR_GAP * 3 + text.line_height();

    renderer.SetDrawColor(STATS_PANEL_BG.r, STATS_PANEL_BG.g, STATS_PANEL_BG.b,
                          STATS_PANEL_BG.a);
    renderer.FillRect(SDL2pp::Rect(x - PANEL_PAD, y - PANEL_PAD,
                                   BAR_W + PANEL_PAD * 2, panel_h));

    const float vida_ratio =
        s.vida_max > 0 ? static_cast<float>(s.vida) / s.vida_max : 0.0f;
    draw_bar(x, y, BAR_W, BAR_H, vida_ratio, colors::HEALTH_BAR);
    text.draw("Vida " + std::to_string(s.vida) + "/" +
                  std::to_string(s.vida_max),
              x + PANEL_PAD, y - 1, colors::WHITE);

    const int y2 = y + BAR_H + BAR_GAP;
    const float mana_ratio =
        s.mana_max > 0 ? static_cast<float>(s.mana) / s.mana_max : 0.0f;
    draw_bar(x, y2, BAR_W, BAR_H, mana_ratio, colors::MANA_BAR);
    text.draw("Mana " + std::to_string(s.mana) + "/" +
                  std::to_string(s.mana_max),
              x + PANEL_PAD, y2 - 1, colors::WHITE);

    const int y3 = y2 + BAR_H + BAR_GAP;
    text.draw("Nivel " + std::to_string(s.nivel) + "   Exp " +
                  std::to_string(s.experiencia) + "   Oro " +
                  std::to_string(s.oro),
              x, y3, colors::WHITE);
}

void HudRenderer::draw_frame() {
    if (!tex_frame) {
        return;
    }
    const SDL2pp::Point out = renderer.GetOutputSize();
    const SDL_Rect full{0, 0, out.x, out.y};
    SDL_RenderCopy(renderer.Get(), tex_frame, nullptr, &full);
}

void HudRenderer::render(const ClientGameState& state) {
    draw_error_toast(state);
    if (state.is_inventory_open()) draw_inventory(state);

    if (!state.has_local_stats()) {
        return;
    }
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    draw_stats(state.get_local_stats());
    draw_frame();
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
}

HudRenderer::~HudRenderer() {
    if (tex_frame) {
        SDL_DestroyTexture(tex_frame);
    }
}
