#include "render/hud_renderer.h"

#include <SDL2/SDL_image.h>

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <string>

#include "render/asset_paths.h"
#include "render/colors.h"

static constexpr Uint32 TOAST_MS = 4000;
static constexpr int TOAST_PAD = 8;
static constexpr int TOAST_MARGIN_TOP = 12;

static constexpr int PANEL_BORDER_W = 2;
static constexpr int PANEL_PAD = 12;

static constexpr int BAR_H = 20;
static constexpr int BAR_GAP = 8;
static constexpr int BAR_TEXT_PAD = 8;

static constexpr int CHAT_PAD = 6;
static constexpr int CHAT_LINE_GAP = 2;
static constexpr int CHAT_MAX_LINES = 8;
static constexpr int CHAT_FONT_SIZE = 13;
static constexpr int CHAT_BOTTOM_MARGIN = 10;

static constexpr int HEADER_GAP = 2;
static constexpr int SECTION_GAP = 14;
static constexpr int LINE_GAP = 4;

static constexpr SDL_Color SLOT_BG = {40, 34, 46, 255};
static constexpr int INV_CELL = 36;
static constexpr int INV_CELL_GAP = 4;
static constexpr int INV_ICON_PAD = 3;
static constexpr int QTY_PAD = 2;

static constexpr SDL_Color BAR_TRACK{40, 40, 40, 220};

SDL_Texture* HudRenderer::load_texture(const std::string& rel_path) const {
    const auto full =
        (std::filesystem::current_path() / config.assets_path / rel_path)
            .lexically_normal();
    SDL_Texture* tex = nullptr;
    if (SDL_Surface* surf = IMG_Load(full.string().c_str())) {
        tex = SDL_CreateTextureFromSurface(renderer.Get(), surf);
        SDL_FreeSurface(surf);
    }
    return tex;
}

HudRenderer::HudRenderer(SDL2pp::Renderer& renderer,
                         const ClientConfig& config):
    renderer(renderer),
    config(config),
    text(
        renderer.Get(),
        (std::filesystem::current_path() / config.font_path).lexically_normal(),
        config.font_size),
    chat_text(
        renderer.Get(),
        (std::filesystem::current_path() / config.font_path).lexically_normal(),
        CHAT_FONT_SIZE),
    item_sprites(renderer, config) {
    hud_bg = load_texture(std::string(assets::HUD_BG));
    slot_frame = load_texture(std::string(assets::INV_SLOT_FRAME));
    frame_tex = load_texture(std::string(assets::UI_MARCO));
}

void HudRenderer::draw_frame() {
    if (!frame_tex) {
        return;
    }
    const SDL_Rect dst{0, 0, config.game_area_width(), config.window_height};
    SDL_RenderCopy(renderer.Get(), frame_tex, nullptr, &dst);
}

void HudRenderer::draw_panel() {
    const int panel_x = config.game_area_width();
    const int panel_w = config.hud_panel_width();
    const int h = config.window_height;

    if (hud_bg) {
        const SDL_Rect dst{panel_x, 0, panel_w, h};
        SDL_RenderCopy(renderer.Get(), hud_bg, nullptr, &dst);
    } else {
        renderer.SetDrawColor(colors::HUD_PANEL_BG.r, colors::HUD_PANEL_BG.g,
                              colors::HUD_PANEL_BG.b, colors::HUD_PANEL_BG.a);
        renderer.FillRect(SDL2pp::Rect(panel_x, 0, panel_w, h));
    }

    renderer.SetDrawColor(
        colors::HUD_PANEL_BORDER.r, colors::HUD_PANEL_BORDER.g,
        colors::HUD_PANEL_BORDER.b, colors::HUD_PANEL_BORDER.a);
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
    text.draw(
        "Vida " + std::to_string(s.vida) + "/" + std::to_string(s.vida_max),
        x + BAR_TEXT_PAD, ty, colors::WHITE);
    y += BAR_H + BAR_GAP;

    const float mana_ratio =
        s.mana_max > 0 ? static_cast<float>(s.mana) / s.mana_max : 0.0f;
    draw_bar(x, y, w, BAR_H, mana_ratio, colors::MANA_BAR);
    ty = y + (BAR_H - text.line_height()) / 2;
    text.draw(
        "Mana " + std::to_string(s.mana) + "/" + std::to_string(s.mana_max),
        x + BAR_TEXT_PAD, ty, colors::WHITE);
    y += BAR_H + SECTION_GAP;
}

void HudRenderer::draw_resources(const PlayerStats& s, int x, int& y) {
    text.draw("Exp  " + std::to_string(s.experiencia) + " / " +
                  std::to_string(s.exp_limite),
              x, y, colors::WHITE);
    y += text.line_height() + LINE_GAP;
    text.draw("Oro  " + std::to_string(s.oro), x, y, colors::ITEM_EQUIPPED);
    y += text.line_height() + SECTION_GAP;
}

void HudRenderer::draw_inventory_slot(const InventorySlotView& slot, int index,
                                      int cx, int cy, int cell) {
    if (slot_frame) {
        const SDL_Rect dst{cx, cy, cell, cell};
        SDL_RenderCopy(renderer.Get(), slot_frame, nullptr, &dst);
    } else {
        renderer.SetDrawColor(SLOT_BG.r, SLOT_BG.g, SLOT_BG.b, SLOT_BG.a);
        renderer.FillRect(SDL2pp::Rect(cx, cy, cell, cell));
        renderer.SetDrawColor(
            colors::HUD_PANEL_BORDER.r, colors::HUD_PANEL_BORDER.g,
            colors::HUD_PANEL_BORDER.b, colors::HUD_PANEL_BORDER.a);
        renderer.DrawRect(SDL2pp::Rect(cx, cy, cell, cell));
    }

    const std::string idx = std::to_string(index);
    chat_text.draw(idx, cx + 2, cy + 1, colors::TEXT_GRAY);

    if (slot.empty()) {
        return;
    }

    if (const ItemSprite* spr = item_sprites.find(slot.item)) {
        const int pad = INV_ICON_PAD + spr->extra_pad;
        const SDL_Rect dst{cx + pad, cy + pad, cell - 2 * pad, cell - 2 * pad};
        SDL_RenderCopy(renderer.Get(), spr->tex, &spr->src, &dst);
    }

    if (slot.cantidad > 1) {
        const std::string n = std::to_string(slot.cantidad);
        int tw = 0, th = 0;
        text.size_text(n, tw, th);
        text.draw(n, cx + cell - tw - QTY_PAD, cy + cell - th, colors::WHITE);
    }

    if (slot.equipado) {
        renderer.SetDrawColor(colors::ITEM_EQUIPPED.r, colors::ITEM_EQUIPPED.g,
                              colors::ITEM_EQUIPPED.b, colors::ITEM_EQUIPPED.a);
        renderer.DrawRect(SDL2pp::Rect(cx, cy, cell, cell));
    }
}

void HudRenderer::draw_inventory_section(const ClientGameState& state, int x,
                                         int w, int y) {
    text.draw("INVENTARIO", x, y, colors::TEXT_GRAY);
    y += text.line_height() + LINE_GAP;

    const auto& slots = state.get_inventory();
    const int step = INV_CELL + INV_CELL_GAP;
    const int cols = std::max(1, (w + INV_CELL_GAP) / step);

    inv_origin_x = x;
    inv_origin_y = y;
    inv_cols = cols;
    inv_count = static_cast<int>(slots.size());

    for (size_t i = 0; i < slots.size(); ++i) {
        const int col = static_cast<int>(i) % cols;
        const int row = static_cast<int>(i) / cols;
        draw_inventory_slot(slots[i], static_cast<int>(i), x + col * step,
                            y + row * step, INV_CELL);
    }
}

int HudRenderer::slot_at(int mouse_x, int mouse_y) const {
    const int step = INV_CELL + INV_CELL_GAP;
    const int rel_x = mouse_x - inv_origin_x;
    const int rel_y = mouse_y - inv_origin_y;
    if (rel_x < 0 || rel_y < 0) {
        return -1;
    }
    const int col = rel_x / step;
    const int row = rel_y / step;
    if (col >= inv_cols) {
        return -1;
    }
    if (rel_x % step >= INV_CELL || rel_y % step >= INV_CELL) {
        return -1;
    }
    const int idx = row * inv_cols + col;
    if (idx < 0 || idx >= inv_count) {
        return -1;
    }
    return idx;
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

void HudRenderer::draw_chat_panel(const ClientGameState& state,
                                  const Console& console) {
    const auto& msgs = state.get_chat_messages();
    const bool open = console.is_open();

    const int chat_line_h = chat_text.line_height() + CHAT_LINE_GAP;
    const int shown = std::min(static_cast<int>(msgs.size()), CHAT_MAX_LINES);
    const int input_rows = open ? 1 : 0;
    const int total_rows = std::max(1, shown + input_rows);

    const int panel_x = config.game_area_width() + PANEL_PAD;
    const int panel_w = config.hud_panel_width() - PANEL_PAD * 2;
    const int box_h = total_rows * chat_line_h + CHAT_PAD * 2;
    const int box_y = config.window_height - box_h - CHAT_BOTTOM_MARGIN;

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(colors::CHAT_BG.r, colors::CHAT_BG.g,
                          colors::CHAT_BG.b, colors::CHAT_BG.a);
    renderer.FillRect(SDL2pp::Rect(panel_x, box_y, panel_w, box_h));
    renderer.SetDrawColor(
        colors::HUD_PANEL_BORDER.r, colors::HUD_PANEL_BORDER.g,
        colors::HUD_PANEL_BORDER.b, colors::HUD_PANEL_BORDER.a);
    renderer.DrawRect(SDL2pp::Rect(panel_x, box_y, panel_w, box_h));
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);

    int ly = box_y + CHAT_PAD;
    const int total = static_cast<int>(msgs.size());
    const int max_scroll = std::max(0, total - CHAT_MAX_LINES);
    if (chat_scroll > max_scroll) {
        chat_scroll = max_scroll;
    }
    const int end = total - chat_scroll;
    const int start = std::max(0, end - shown);
    const int max_text_w = panel_w - CHAT_PAD * 2;

    for (int i = start; i < end; ++i) {
        std::string line = msgs[i].from + ": " + msgs[i].text;
        int tw = 0, th = 0;
        chat_text.size_text(line, tw, th);
        while (tw > max_text_w && line.size() > 1) {
            line.pop_back();
            chat_text.size_text(line, tw, th);
        }
        chat_text.draw(line, panel_x + CHAT_PAD, ly, colors::WHITE);
        ly += chat_line_h;
    }

    if (open) {
        chat_text.draw("> " + console.current() + "_", panel_x + CHAT_PAD, ly,
                       colors::ITEM_EQUIPPED);
    }
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

void HudRenderer::scroll_chat(int delta, int total_msgs) {
    const int max_scroll = std::max(0, total_msgs - CHAT_MAX_LINES);
    chat_scroll += delta;
    if (chat_scroll < 0) chat_scroll = 0;
    if (chat_scroll > max_scroll) chat_scroll = max_scroll;
}

void HudRenderer::render(const ClientGameState& state, const Console& console) {
    draw_frame();
    draw_panel();
    draw_chat_panel(state, console);
    draw_error_toast(state);

    if (!state.has_local_stats()) {
        return;
    }
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    draw_player_panel(state);
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
}

HudRenderer::~HudRenderer() {
    if (hud_bg) SDL_DestroyTexture(hud_bg);
    if (slot_frame) SDL_DestroyTexture(slot_frame);
    if (frame_tex) SDL_DestroyTexture(frame_tex);
}
