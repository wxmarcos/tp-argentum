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

inline constexpr const char* PATH_ATTACK = "assets/sonidos/hacer_ataque.wav";
inline constexpr const char* PATH_DEATH = "assets/sonidos/muerte.wav";
inline constexpr const char* PATH_MEDITATE = "assets/sonidos/meditar.wav";
inline constexpr const char* PATH_HIT = "assets/sonidos/recibir_golpe.wav";
inline constexpr const char* PATH_STEP = "assets/sonidos/paso_caminar.wav";
inline constexpr const char* PATH_LEVELUP = "assets/sonidos/subir_nivel.wav";

inline constexpr int STEP_VOLUME = 13;

inline constexpr int MAX_SOUNDS_PER_FRAME = 3;

}

#endif