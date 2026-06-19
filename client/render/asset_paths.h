#ifndef CLIENT_RENDER_ASSET_PATHS_H
#define CLIENT_RENDER_ASSET_PATHS_H

#include <string_view>

namespace assets {

// UI
inline constexpr std::string_view UI_INICIO = "ui/inicio.png";
inline constexpr std::string_view UI_LOGIN  = "ui/login.png";
inline constexpr std::string_view UI_CREATE = "ui/create_character.png";
inline constexpr std::string_view UI_ICON   = "ui/icon.png";
inline constexpr std::string_view UI_MARCO  = "ui/marco.png";
inline constexpr std::string_view HUD_BG = "ui/hudFondo.png";
inline constexpr std::string_view INV_SLOT_FRAME = "ui/bordeInv.png";
inline constexpr std::string_view UI_CURSOR = "ui/cursor.png";

// Cuerpos por raza
inline constexpr std::string_view BODY_HUMANO =
    "cuerpos/razas/humanoCuerpo.png";
inline constexpr std::string_view BODY_FANTASMA = "cuerpos/fantasmin.png";
inline constexpr std::string_view BODY_ELFO = "cuerpos/razas/elfoCuerpo.png";
inline constexpr std::string_view BODY_ENANO = "cuerpos/razas/enanoCuerpo.png";
inline constexpr std::string_view BODY_GNOMO = "cuerpos/razas/gnomoCuerpo.png";

// Cuerpos por clase
inline constexpr std::string_view BODY_MAGO = "cuerpos/clases/magoCuerpo.png";
inline constexpr std::string_view BODY_CLERIGO =
    "cuerpos/clases/clerigoCuerpo.png";
inline constexpr std::string_view BODY_GUERRERO =
    "cuerpos/clases/guerreroCuerpo.png";
inline constexpr std::string_view BODY_PALADIN =
    "cuerpos/clases/paladinCuerpo.png";

// Criaturas
inline constexpr std::string_view CRE_GOBLIN = "criaturas/goblin.png";
inline constexpr std::string_view CRE_ESQUELETO = "criaturas/esqueleto.png";
inline constexpr std::string_view CRE_ZOMBIE = "criaturas/zombie.png";
inline constexpr std::string_view CRE_ORCO = "criaturas/orco.png";
inline constexpr std::string_view CRE_ARANA = "criaturas/arana.png";
inline constexpr std::string_view CRE_GOLEM = "criaturas/golem.png";

// NPCs
inline constexpr std::string_view NPC_BANQUERO = "characters/banquero.png";
inline constexpr std::string_view NPC_COMERCIANTE =
    "characters/comerciante.png";
inline constexpr std::string_view NPC_SACERDOTE = "characters/sacerdote.png";

// Cabezas
inline constexpr std::string_view HEAD_HUMANO = "cabezas/humanoCabezas.png";
inline constexpr std::string_view HEAD_ELFO = "cabezas/elfoCabezas.png";
inline constexpr std::string_view HEAD_ENANO = "cabezas/enanoCabezas.png";
inline constexpr std::string_view HEAD_GNOMO = "cabezas/gnomoCabezas.png";

// Armaduras
inline constexpr std::string_view ARMADURA_CUERO =
    "objetos/armaduras/armaduraCuero.png";
inline constexpr std::string_view ARMADURA_PLACAS =
    "objetos/armaduras/armaduraPlacas.png";
inline constexpr std::string_view TUNICA_AZUL =
    "objetos/armaduras/tunicaAzul.png";

// Cascos
inline constexpr std::string_view CASCO_CAPUCHA = "objetos/armaduras/capucha.png";
inline constexpr std::string_view CASCO_HIERRO = "objetos/armaduras/cascoHierro.png";
inline constexpr std::string_view CASCO_SOMBRERO_MAGICO = "objetos/armaduras/sombreroMagico.png";

// Armas
inline constexpr std::string_view ESPADA = "objetos/armas/espada.png";
inline constexpr std::string_view HACHA = "objetos/armas/hacha.png";
inline constexpr std::string_view MARTILLO = "objetos/armas/martillo.png";
inline constexpr std::string_view VARADEFRESNO = "objetos/armas/varaDeFresno.png";
inline constexpr std::string_view FLAUTAELFICA = "objetos/armas/flautaElfica.png";
inline constexpr std::string_view BACULONUDOSO = "objetos/armas/baculoNudoso.png";
inline constexpr std::string_view BACULOENGARZADO =
    "objetos/armas/baculoEngarzado.png";
inline constexpr std::string_view ARCOSIMPLE = "objetos/armas/arcoSimple.png";
inline constexpr std::string_view ARCOCOMPUESTO =
    "objetos/armas/arcoCompuesto.png";
inline constexpr std::string_view ESCUDOTORTUGA =
    "objetos/armas/escudoTortuga.png";
inline constexpr std::string_view ESCUDOHIERRO = "objetos/armas/escudoHierro.png";

}

#endif