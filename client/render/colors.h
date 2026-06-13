#ifndef CLIENT_RENDER_COLORS_H
#define CLIENT_RENDER_COLORS_H

#include <SDL2/SDL.h>

namespace colors {

inline constexpr SDL_Color WHITE         = {255, 255, 255, 255};
inline constexpr SDL_Color BLACK         = {0, 0, 0, 255};
inline constexpr SDL_Color TEXT_GRAY     = {180, 180, 180, 255};
inline constexpr SDL_Color ERROR_TEXT    = {255, 230, 230, 255};
inline constexpr SDL_Color HEALTH_BAR    = {200, 40, 40, 255};
inline constexpr SDL_Color MANA_BAR      = {40, 90, 220, 255};
inline constexpr SDL_Color ITEM_EQUIPPED = {255, 220, 60, 255};
inline constexpr SDL_Color DMG_CRIT      = {255, 220, 40, 255};
inline constexpr SDL_Color DMG_RECEIVED  = {230, 60, 60, 255};
inline constexpr SDL_Color DODGE         = {180, 220, 255, 255};

inline constexpr SDL_Color HUD_PANEL_BG     = {26, 22, 30, 255};
inline constexpr SDL_Color HUD_PANEL_BORDER = {84, 72, 96, 255};

inline constexpr SDL_Color CHAT_BG = {0, 0, 0, 150};

inline constexpr SDL_Color MENU_BG{20, 20, 25, 255};
inline constexpr SDL_Color SELECT_FILL{255, 200, 80, 60};
inline constexpr SDL_Color SELECT_BORDER{255, 210, 110, 255};
inline constexpr SDL_Color ERROR_TOAST_BG = {120, 20, 20, 210};

}

#endif