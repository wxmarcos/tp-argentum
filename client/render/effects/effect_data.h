#ifndef CLIENT_RENDER_EFFECT_DATA_H
#define CLIENT_RENDER_EFFECT_DATA_H

#include <SDL2/SDL.h>

#include <cstdint>
#include <string_view>
#include <vector>

#include "render/effects/effect_kind.h"

struct ActiveEffect {
    EffectKind kind;
    int wx;
    int wy;
    uint32_t age_ms;
};

struct EffectData {
    std::vector<SDL_Rect> frames;
    int ms_per_frame = 80;
    bool loop = false;
};

struct EffectDef {
    EffectKind kind;
    EffectData (*make)();
    std::string_view sheet;
};

EffectData make_meditar_effect_data();
EffectData make_efectoMorir_effect_data();
EffectData make_resucitar_effect_data();
EffectData make_explosionSuper_effect_data();
EffectData make_explosionComun_effect_data();
EffectData make_curarse_effect_data();
EffectData make_ataqueBaculoComun_effect_data();
EffectData make_ataqueBaculoDorado_effect_data();
EffectData make_ataqueComunRojo_effect_data();
EffectData make_ataqueComunGris_effect_data();
EffectData make_ataqueComunDorado_effect_data();
EffectData make_ataqueFlechas_effect_data();

std::vector<EffectDef> all_effect_defs();

#endif
