#ifndef CLIENT_RENDER_ASSET_PATHS_H
#define CLIENT_RENDER_ASSET_PATHS_H

#include <string_view>

namespace assets {

// UI
inline constexpr std::string_view UI_INICIO = "ui/inicio.png";
inline constexpr std::string_view UI_LOGIN = "ui/login.png";
inline constexpr std::string_view UI_CREATE = "ui/create_character.png";
inline constexpr std::string_view UI_ICON = "ui/icon.png";
inline constexpr std::string_view UI_MARCO = "ui/marco.png";
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
inline constexpr std::string_view CRE_ESQUELETO_HACHA =
    "criaturas/esqueletoHacha.png";
inline constexpr std::string_view CRE_ARANA_BLANCA =
    "criaturas/aranaBlanca.png";
inline constexpr std::string_view CRE_GOBLIN_JOROBADO =
    "criaturas/goblinJorobado.png";
inline constexpr std::string_view CRE_GOLEM_DEMONIACO =
    "criaturas/golemDemoniaco.png";

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
inline constexpr std::string_view CASCO_CAPUCHA =
    "objetos/armaduras/capucha.png";
inline constexpr std::string_view CASCO_HIERRO =
    "objetos/armaduras/cascoHierro.png";
inline constexpr std::string_view CASCO_SOMBRERO_MAGICO =
    "objetos/armaduras/sombreroMagico.png";

// Armas (animacion de caminata)

inline constexpr std::string_view ESPADA_ANIM =
    "objetos/armas/espadaMovimiento.png";
inline constexpr std::string_view HACHA_ANIM =
    "objetos/armas/hachaMovimiento.png";
inline constexpr std::string_view MARTILLO_ANIM =
    "objetos/armas/martilloMovimiento.png";
inline constexpr std::string_view VARADEFRESNO_ANIM =
    "objetos/armas/varaDeFresnoMovimiento.png";
inline constexpr std::string_view FLAUTAELFICA_ANIM =
    "objetos/armas/flautaElficaMovimiento.png";
inline constexpr std::string_view BACULONUDOSO_ANIM =
    "objetos/armas/baculoNudosoMovimiento.png";
inline constexpr std::string_view BACULOENGARZADO_ANIM =
    "objetos/armas/baculoEngarzadoMovimiento.png";
inline constexpr std::string_view ARCOSIMPLE_ANIM =
    "objetos/armas/arcoSimpleMovimiento.png";
inline constexpr std::string_view ARCOCOMPUESTO_ANIM =
    "objetos/armas/arcoCompuestoMovimiento.png";
inline constexpr std::string_view ESCUDOTORTUGA_ANIM =
    "objetos/armas/escudoTortugaMovimiento.png";
inline constexpr std::string_view ESCUDOHIERRO_ANIM =
    "objetos/armas/escudoHierroMovimiento.png";

}  // namespace assets

#endif
