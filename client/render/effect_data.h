#ifndef CLIENT_RENDER_EFFECT_DATA_H
#define CLIENT_RENDER_EFFECT_DATA_H

#include <SDL2/SDL.h>
#include "render/effect_kind.h"

#include <vector>

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

inline EffectData make_meditar_effect_data() {
    EffectData d;
    d.ms_per_frame = 100;
    d.loop = true;
    d.frames = {
        {34, 99, 124, 111},   {226, 96, 124, 111},  {418, 92, 124, 111},
        {610, 87, 124, 111},  {802, 83, 124, 111},  {34, 269, 124, 111},
        {226, 264, 124, 111}, {418, 261, 124, 111}, {610, 260, 124, 111},
        {802, 261, 124, 111}, {34, 453, 124, 111},  {226, 453, 124, 111},
        {418, 454, 124, 111}, {610, 455, 124, 111}, {803, 455, 124, 111},
    };
    return d;
}

inline EffectData make_efectoMorir_effect_data() {
    EffectData d;
    d.ms_per_frame = 90;
    d.loop = false;
    d.frames = {
        {7, 6, 82, 114},     {104, 6, 82, 114},   {200, 6, 82, 114},
        {296, 4, 82, 114},   {392, 4, 82, 114},   {8, 120, 82, 114},
        {104, 130, 82, 114}, {198, 130, 82, 114}, {293, 129, 82, 114},
        {390, 129, 82, 114},
    };
    return d;
}

inline EffectData make_resucitar_effect_data() {
    EffectData d;
    d.ms_per_frame = 90;
    d.loop = false;
    d.frames = {
        {1, 4, 92, 180},   {93, 0, 92, 180},  {184, 0, 92, 180},
        {276, 0, 92, 180}, {367, 0, 92, 180},
    };
    return d;
}

inline EffectData make_explosionSuper_effect_data() {
    EffectData d;
    d.ms_per_frame = 70;
    d.loop = false;
    d.frames = {
        {1, 19, 143, 140},  {146, 2, 143, 140},   {289, 0, 143, 140},
        {1, 137, 143, 140}, {145, 146, 143, 140}, {292, 146, 143, 140},
        {0, 292, 143, 140},
    };
    return d;
}

inline EffectData make_ataqueBaculoComun_effect_data() {
    EffectData d;
    d.ms_per_frame = 60;
    d.loop = false;
    d.frames = {
        {0, 0, 132, 152},     {136, 0, 132, 152},   {274, 8, 132, 152},
        {410, 0, 132, 152},   {0, 150, 132, 152},   {137, 149, 132, 152},
        {277, 150, 132, 152}, {405, 150, 132, 152}, {1, 305, 132, 152},
        {137, 304, 132, 152}, {274, 305, 132, 152}, {409, 307, 132, 152},
        {2, 467, 132, 152},   {137, 459, 132, 152}, {273, 460, 132, 152},
    };
    return d;
}

inline EffectData make_curarse_effect_data() {
    EffectData d;
    d.ms_per_frame = 70;
    d.loop = false;
    d.frames = {
        {25, 23, 142, 151},   {203, 16, 142, 151},  {407, 20, 142, 151},
        {597, 22, 142, 151},  {790, 20, 142, 151},  {26, 215, 142, 151},
        {217, 215, 142, 151}, {409, 215, 142, 151}, {602, 215, 142, 151},
        {794, 215, 142, 151}, {25, 407, 142, 151},  {215, 406, 142, 151},
        {405, 404, 142, 151}, {595, 401, 142, 151}, {791, 399, 142, 151},
        {19, 590, 142, 151},  {209, 589, 142, 151}, {410, 589, 142, 151},
        {604, 586, 142, 151}, {798, 605, 142, 151}, {31, 795, 142, 151},
        {236, 782, 142, 151}, {430, 778, 142, 151}, {604, 777, 142, 151},
        {793, 775, 142, 151},
    };
    return d;
}

inline EffectData make_explosionComun_effect_data() {
    EffectData d;
    d.ms_per_frame = 70;
    d.loop = false;
    d.frames = {
        {12, 76, 120, 82},  {158, 76, 120, 82},  {302, 76, 120, 82},
        {13, 220, 120, 82}, {158, 219, 120, 82}, {303, 217, 120, 82},
        {12, 352, 120, 82}, {156, 347, 120, 82}, {303, 346, 120, 82},
    };
    return d;
}

inline EffectData make_ataqueBaculoDorado_effect_data() {
    EffectData d;
    d.ms_per_frame = 70;
    d.loop = false;
    d.frames = {
        {0, 3, 112, 117},     {112, 4, 112, 117},   {224, 3, 112, 117},
        {336, 2, 112, 117},   {447, 3, 112, 117},   {559, 3, 112, 117},
        {0, 117, 112, 117},   {112, 113, 112, 117}, {224, 117, 112, 117},
        {336, 118, 112, 117}, {447, 118, 112, 117}, {541, 101, 112, 117},
    };
    return d;
}

inline EffectData make_ataqueComunRojo_effect_data() {
    EffectData d;
    d.ms_per_frame = 70;
    d.loop = false;
    d.frames = {
        {2, 13, 69, 64},   {88, 27, 69, 64},  {156, 23, 69, 64},
        {224, 28, 69, 64}, {314, 42, 69, 64}, {381, 49, 69, 64},
    };
    return d;
}

inline EffectData make_ataqueComunGris_effect_data() {
    EffectData d;
    d.ms_per_frame = 70;
    d.loop = false;
    d.frames = {
        {19, 99, 69, 64},  {88, 101, 69, 64}, {156, 102, 69, 64},
        {225, 89, 69, 64}, {294, 88, 69, 64}, {362, 87, 69, 64},
    };
    return d;
}

inline EffectData make_ataqueComunDorado_effect_data() {
    EffectData d;
    d.ms_per_frame = 70;
    d.loop = false;
    d.frames = {
        {25, 175, 69, 43},  {88, 176, 69, 43},  {156, 174, 69, 43},
        {226, 173, 69, 43}, {293, 179, 69, 43},
    };
    return d;
}

struct EffectDef {
    EffectKind kind;
    EffectData (*make)();
    const char* sheet;
};

inline std::vector<EffectDef> all_effect_defs() {
    return {
        {EffectKind::Meditar, make_meditar_effect_data, "efectos/meditar.png"},
        {EffectKind::EfectoMorir, make_efectoMorir_effect_data,
         "efectos/efectoMorir.png"},
        {EffectKind::Resucitar, make_resucitar_effect_data,
         "efectos/resucitar.png"},
        {EffectKind::ExplosionSuper, make_explosionSuper_effect_data,
         "efectos/explosionSuper.png"},
        {EffectKind::ExplosionComun, make_explosionComun_effect_data,
         "efectos/explosionComun.png"},
        {EffectKind::Curarse, make_curarse_effect_data, "efectos/curarse.png"},
        {EffectKind::AtaqueBaculoComun, make_ataqueBaculoComun_effect_data,
         "efectos/ataqueBaculoComun.png"},
        {EffectKind::AtaqueBaculoDorado, make_ataqueBaculoDorado_effect_data,
         "efectos/ataqueBaculoDorado.png"},
        {EffectKind::AtaqueComunRojo, make_ataqueComunRojo_effect_data,
         "efectos/ataqueComun.png"},
        {EffectKind::AtaqueComunGris, make_ataqueComunGris_effect_data,
         "efectos/ataqueComun.png"},
        {EffectKind::AtaqueComunDorado, make_ataqueComunDorado_effect_data,
         "efectos/ataqueComun.png"},
    };
}

#endif
