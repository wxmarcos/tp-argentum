#include "render/hud_renderer.h"

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <string>

#include "render/colors.h"

static constexpr Uint32 TOAST_MS = 4000;
static constexpr int TOAST_PAD = 8;
static constexpr int TOAST_MARGIN_TOP = 12;

static constexpr int PANEL_BORDER_W = 2;
static constexpr int PANEL_PAD = 12;

static constexpr int BAR_H = 20;
static constexpr int BAR_GAP = 8;
static constexpr int BAR_TEXT_PAD = 8;

static constexpr int CHAT_MARGIN = 10;
static constexpr int CHAT_PAD = 6;
static constexpr int CHAT_WIDTH = 460;
static constexpr int CHAT_LINE_GAP = 2;
static constexpr int CHAT_MAX_LINES = 8;

static constexpr int HEADER_GAP = 2;
static constexpr int SECTION_GAP = 14;
static constexpr int LINE_GAP = 4;
static constexpr int ROW_EXTRA = 4;

static constexpr SDL_Color BAR_TRACK{40, 40, 40, 220};
static constexpr SDL_Color EQUIPPED_ROW{60, 80, 40, 120};

HudRenderer::HudRenderer(SDL2pp::Renderer& renderer,
                         const ClientConfig& config):
        renderer(renderer),
        config(config),
        text(renderer.Get(),
             (std::filesystem::current_path() / config.font_path)
                 .lexically_normal(),
             config.font_size) {}

void HudRenderer::draw_panel() {
    const int panel_x = config.game_area_width();
    const int panel_w = config.hud_panel_width();
    const int h = config.window_height;

    renderer.SetDrawColor(colors::HUD_PANEL_BG.r, colors::HUD_PANEL_BG.g,
                          colors::HUD_PANEL_BG.b, colors::HUD_PANEL_BG.a);
    renderer.FillRect(SDL2pp::Rect(panel_x, 0, panel_w, h));

    renderer.SetDrawColor(colors::HUD_PANEL_BORDER.r, colors::HUD_PANEL_BORDER.g,
                          colors::HUD_PANEL_BORDER.b,
                          colors::HUD_PANEL_BORDER.a);
    renderer.FillRect(SDL2pp::Rect(panel_x, 0, PANEL_BORDER_W, h));
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

void HudRenderer::draw_header(const PlayerStats& s, const std::string& name,
                              int x, int& y) {
    text.draw(name, x, y, colors::WHITE);
    y += text.line_height() + HEADER_GAP;

    const std::string sub = s.clase + "  -  Nivel " + std::to_string(s.nivel);
    text.draw(sub, x, y, colors::TEXT_GRAY);
    y += text.line_height() + SECTION_GAP;
}

void HudRenderer::draw_vitals(const PlayerStats& s, int x, int w, int& y) {
    const float vida_ratio =
        s.vida_max > 0 ? static_cast<float>(s.vida) / s.vida_max : 0.0f;
    draw_bar(x, y, w, BAR_H, vida_ratio, colors::HEALTH_BAR);
    int ty = y + (BAR_H - text.line_height()) / 2;
    text.draw("Vida " + std::to_string(s.vida) + "/" +
                  std::to_string(s.vida_max),
              x + BAR_TEXT_PAD, ty, colors::WHITE);
    y += BAR_H + BAR_GAP;

    const float mana_ratio =
        s.mana_max > 0 ? static_cast<float>(s.mana) / s.mana_max : 0.0f;
    draw_bar(x, y, w, BAR_H, mana_ratio, colors::MANA_BAR);
    ty = y + (BAR_H - text.line_height()) / 2;
    text.draw("Mana " + std::to_string(s.mana) + "/" +
                  std::to_string(s.mana_max),
              x + BAR_TEXT_PAD, ty, colors::WHITE);
    y += BAR_H + SECTION_GAP;
}

void HudRenderer::draw_resources(const PlayerStats& s, int x, int& y) {
    text.draw("Exp  " + std::to_string(s.experiencia), x, y, colors::WHITE);
    y += text.line_height() + LINE_GAP;
    text.draw("Oro  " + std::to_string(s.oro), x, y, colors::ITEM_EQUIPPED);
    y += text.line_height() + SECTION_GAP;
}

void HudRenderer::draw_inventory_slot(const InventorySlotView& slot,
                                      size_t idx, int x, int w, int row_y) {
    if (!slot.empty() && slot.equipado) {
        renderer.SetDrawColor(EQUIPPED_ROW.r, EQUIPPED_ROW.g, EQUIPPED_ROW.b,
                              EQUIPPED_ROW.a);
        renderer.FillRect(
            SDL2pp::Rect(x - 2, row_y, w + 4, text.line_height()));
    }

    std::string line = "[" + std::to_string(idx) + "] ";
    if (slot.empty()) {
        text.draw(line + "(vacio)", x, row_y, colors::TEXT_GRAY);
        return;
    }
    line += slot.item;
    if (slot.cantidad > 1) line += " x" + std::to_string(slot.cantidad);
    if (slot.equipado) line += " [E]";
    text.draw(line, x, row_y,
              slot.equipado ? colors::ITEM_EQUIPPED : colors::WHITE);
}

void HudRenderer::draw_inventory_section(const ClientGameState& state, int x,
                                         int w, int y) {
    text.draw("INVENTARIO", x, y, colors::TEXT_GRAY);
    y += text.line_height() + LINE_GAP;

    const auto& slots = state.get_inventory();
    const int row_h = text.line_height() + ROW_EXTRA;
    for (size_t i = 0; i < slots.size(); ++i) {
        const int row_y = y + static_cast<int>(i) * row_h;
        draw_inventory_slot(slots[i], i, x, w, row_y);
    }
}

void HudRenderer::draw_player_panel(const ClientGameState& state) {
    const PlayerStats& s = state.get_local_stats();
    const int x = config.game_area_width() + PANEL_PAD;
    const int w = config.hud_panel_width() - PANEL_PAD * 2;
    int y = PANEL_PAD;

    draw_header(s, state.get_local_nick(), x, y);
    draw_vitals(s, x, w, y);
    draw_resources(s, x, y);
    draw_inventory_section(state, x, w, y);
}

void HudRenderer::draw_console(const ClientGameState& state,
                              const Console& console) {
    const auto& msgs = state.get_chat_messages();
    const bool open = console.is_open();
    if (msgs.empty() && !open) {
        return;
    }

    const int line_h = text.line_height() + CHAT_LINE_GAP;
    const int shown = std::min(static_cast<int>(msgs.size()), CHAT_MAX_LINES);
    const int rows = shown + (open ? 1 : 0);

    const int x = CHAT_MARGIN;
    const int y = CHAT_MARGIN;
    const int box_h = rows * line_h + CHAT_PAD * 2;

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(colors::CHAT_BG.r, colors::CHAT_BG.g,
                          colors::CHAT_BG.b, colors::CHAT_BG.a);
    renderer.FillRect(SDL2pp::Rect(x, y, CHAT_WIDTH, box_h));

    int ly = y + CHAT_PAD;
    const int start = static_cast<int>(msgs.size()) - shown;
    for (int i = start; i < static_cast<int>(msgs.size()); ++i) {
        const std::string line = msgs[i].from + ": " + msgs[i].text;
        text.draw(line, x + CHAT_PAD, ly, colors::WHITE);
        ly += line_h;
    }

    if (open) {
        text.draw("> " + console.current() + "_", x + CHAT_PAD, ly,
                  colors::ITEM_EQUIPPED);
    }
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
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
    const int cx = config.game_area_width() / 2;
    const int box_x = cx - box_w / 2;
    const int box_y = TOAST_MARGIN_TOP;

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(colors::ERROR_TOAST_BG.r, colors::ERROR_TOAST_BG.g,
                          colors::ERROR_TOAST_BG.b, colors::ERROR_TOAST_BG.a);
    renderer.FillRect(SDL2pp::Rect(box_x, box_y, box_w, box_h));
    renderer.SetDrawColor(colors::BLACK.r, colors::BLACK.g, colors::BLACK.b,
                          colors::BLACK.a);
    renderer.DrawRect(SDL2pp::Rect(box_x, box_y, box_w, box_h));
    text.draw_centered(msg, cx, box_y + TOAST_PAD, colors::ERROR_TEXT);
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
}

void HudRenderer::render(const ClientGameState& state, const Console& console) {
    draw_panel();
    draw_console(state, console);
    draw_error_toast(state);

    if (!state.has_local_stats()) {
        return;
    }
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    draw_player_panel(state);
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
}
