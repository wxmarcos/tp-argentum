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
inline constexpr std::string_view ESQUELETO_HACHA = "esqueleto_hacha";
inline constexpr std::string_view ARANA_BLANCA = "arana_blanca";
inline constexpr std::string_view GOBLIN_JOROBADO = "goblin_jorobado";
inline constexpr std::string_view GOLEM_DEMONIACO = "golem_demoniaco";

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
inline constexpr std::string_view VARA_DE_FRESNO = "Vara de fresno";
inline constexpr std::string_view FLAUTA_ELFICA = "Flauta elfica";
inline constexpr std::string_view BACULO_NUDOSO = "Baculo nudoso";
inline constexpr std::string_view BACULO_ENGARZADO = "Baculo engarzado";
inline constexpr std::string_view ARCO_SIMPLE = "Arco simple";
inline constexpr std::string_view ARCO_COMPUESTO = "Arco compuesto";

}

#endif