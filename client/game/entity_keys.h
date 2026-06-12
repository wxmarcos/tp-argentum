#ifndef CLIENT_GAME_ENTITY_KEYS_H
#define CLIENT_GAME_ENTITY_KEYS_H

#include <string_view>

namespace keys {

// Razas
inline constexpr std::string_view HUMANO = "humano";
inline constexpr std::string_view ELFO   = "elfo";
inline constexpr std::string_view ENANO  = "enano";
inline constexpr std::string_view GNOMO  = "gnomo";

// Clases
inline constexpr std::string_view MAGO     = "mago";
inline constexpr std::string_view CLERIGO  = "clerigo";
inline constexpr std::string_view GUERRERO = "guerrero";
inline constexpr std::string_view PALADIN  = "paladin";

// Criaturas / NPCs
inline constexpr std::string_view GOBLIN      = "goblin";
inline constexpr std::string_view ESQUELETO   = "esqueleto";
inline constexpr std::string_view ZOMBIE      = "zombie";
inline constexpr std::string_view ARANA       = "arana";
inline constexpr std::string_view ORCO        = "orco";
inline constexpr std::string_view GOLEM       = "golem";
inline constexpr std::string_view BANQUERO    = "banquero";
inline constexpr std::string_view COMERCIANTE = "comerciante";
inline constexpr std::string_view SACERDOTE   = "sacerdote";

// Estado especial de sprite
inline constexpr std::string_view FANTASMA = "fantasma";

}

#endif