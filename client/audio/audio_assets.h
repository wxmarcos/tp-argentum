#ifndef CLIENT_AUDIO_AUDIO_ASSETS_H
#define CLIENT_AUDIO_AUDIO_ASSETS_H

namespace audio_assets {

inline constexpr const char* MUSIC = "assets/sonidos/fondo.mp3";

inline constexpr const char* KEY_ATTACK = "ataque";
inline constexpr const char* KEY_DEATH = "muerte";
inline constexpr const char* KEY_MEDITATE = "meditar";
inline constexpr const char* KEY_HIT = "golpe";
inline constexpr const char* KEY_STEP = "paso";
inline constexpr const char* KEY_LEVELUP = "nivel";
inline constexpr const char* KEY_EQUIP = "equipar";
inline constexpr const char* KEY_DROP = "soltar";
inline constexpr const char* KEY_HEAL    = "curarse";
inline constexpr const char* KEY_REVIVE  = "resucitar";
inline constexpr const char* KEY_STAFF   = "baculo";
inline constexpr const char* KEY_STAFF2 = "baculo2";
inline constexpr const char* KEY_SHOOT = "disparo";

inline constexpr const char* PATH_ATTACK = "assets/sonidos/hacer_ataque.wav";
inline constexpr const char* PATH_DEATH = "assets/sonidos/muerte.wav";
inline constexpr const char* PATH_MEDITATE = "assets/sonidos/meditar.wav";
inline constexpr const char* PATH_HIT = "assets/sonidos/recibir_golpe.wav";
inline constexpr const char* PATH_STEP = "assets/sonidos/paso_caminar.wav";
inline constexpr const char* PATH_LEVELUP = "assets/sonidos/subir_nivel.wav";
inline constexpr const char* PATH_EQUIP = "assets/sonidos/equipar_arma.wav";
inline constexpr const char* PATH_DROP = "assets/sonidos/soltar_item.wav";
inline constexpr const char* PATH_HEAL   = "assets/sonidos/curarse.wav";
inline constexpr const char* PATH_REVIVE = "assets/sonidos/resucitar.wav";
inline constexpr const char* PATH_STAFF  = "assets/sonidos/sonido_baculo.wav";
inline constexpr const char* PATH_STAFF2 = "assets/sonidos/sonido_baculo2.wav";
inline constexpr const char* PATH_SHOOT = "assets/sonidos/disparo.wav";

inline constexpr int STEP_VOLUME = 13;

inline constexpr int MAX_SOUNDS_PER_FRAME = 3;

}

#endif