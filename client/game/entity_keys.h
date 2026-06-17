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

// Estado especial
inline constexpr std::string_view FANTASMA = "fantasma";

// Vestimenta (cuerpos de armadura)
inline constexpr std::string_view ARMADURA_CUERO = "armadura_cuero";
inline constexpr std::string_view ARMADURA_PLACAS = "armadura_placas";
inline constexpr std::string_view TUNICA_AZUL = "tunica_azul";

// Cascos
inline constexpr std::string_view CASCO_CAPUCHA = "casco_capucha";
inline constexpr std::string_view CASCO_HIERRO = "casco_hierro";
inline constexpr std::string_view CASCO_SOMBRERO = "casco_sombrero";

}

namespace items {

inline constexpr std::string_view ARMADURA_CUERO = "Armadura de cuero";
inline constexpr std::string_view ARMADURA_PLACAS = "Armadura de placas";
inline constexpr std::string_view TUNICA_AZUL = "Tunica azul";

inline constexpr std::string_view CAPUCHA = "Capucha";
inline constexpr std::string_view CASCO_HIERRO = "Casco de hierro";
inline constexpr std::string_view SOMBRERO_MAGICO = "Sombrero magico";

inline constexpr std::string_view ESCUDO_TORTUGA = "Escudo de tortuga";
inline constexpr std::string_view ESCUDO_HIERRO = "Escudo de hierro";

}

#endif