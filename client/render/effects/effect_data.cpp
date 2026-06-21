#include "render/effects/effect_data.h"

EffectData make_meditar_effect_data() {
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

EffectData make_efectoMorir_effect_data() {
    EffectData d;
    d.ms_per_frame = 130;
    d.loop = false;
    d.frames = {
        {40, 14, 44, 36},   {154, 10, 72, 79},  {278, 7, 82, 89},
        {402, 8, 104, 87},  {16, 104, 97, 97},  {120, 105, 131, 95},
        {272, 104, 107, 96}, {400, 104, 112, 93}, {13, 200, 95, 96},
        {120, 202, 114, 90},
    };
    return d;
}

EffectData make_resucitar_effect_data() {
    EffectData d;
    d.ms_per_frame = 90;
    d.loop = false;
    d.frames = {
        {1, 2, 92, 93},   {92, 0, 93, 95},  {184, 0, 93, 95},
        {276, 0, 93, 95}, {368, 0, 91, 91},
        {22, 95, 50, 89},  {120, 95, 48, 89}, {220, 95, 38, 83},
        {318, 95, 37, 82}, {419, 105, 23, 66},
    };
    return d;
}

EffectData make_explosionSuper_effect_data() {
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

EffectData make_explosionComun_effect_data() {
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

EffectData make_curarse_effect_data() {
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

EffectData make_ataqueBaculoComun_effect_data() {
    EffectData d;
    d.ms_per_frame = 60;
    d.loop = false;
    d.frames = {
        {49, 56, 30, 34},   {170, 39, 68, 77},  {289, 21, 101, 102},
        {425, 21, 98, 99},
        {13, 178, 101, 94}, {155, 167, 104, 105}, {292, 177, 95, 95},
        {428, 178, 97, 96},
        {12, 331, 104, 99}, {152, 330, 102, 101}, {288, 329, 104, 107},
        {424, 328, 106, 115},
        {14, 483, 104, 105}, {152, 485, 97, 98}, {288, 488, 96, 88},
    };
    return d;
}

EffectData make_ataqueBaculoDorado_effect_data() {
    EffectData d;
    d.ms_per_frame = 70;
    d.loop = false;
    d.frames = {
        {1, 4, 96, 101},   {112, 7, 90, 103},  {213, 4, 95, 109},
        {321, 2, 105, 111}, {435, 5, 115, 112}, {550, 5, 121, 114},
        {0, 113, 114, 120}, {114, 109, 113, 116}, {232, 114, 115, 118},
        {349, 115, 112, 120}, {481, 122, 96, 108},
    };
    return d;
}

EffectData make_ataqueComunRojo_effect_data() {
    EffectData d;
    d.ms_per_frame = 70;
    d.loop = false;
    d.frames = {
        {16, 24, 32, 40},  {87, 34, 25, 38},  {144, 22, 35, 66},
        {208, 26, 31, 62}, {272, 46, 24, 50}, {337, 58, 23, 37},
        {408, 72, 16, 24},
    };
    return d;
}

EffectData make_ataqueComunGris_effect_data() {
    EffectData d;
    d.ms_per_frame = 70;
    d.loop = false;
    d.frames = {
        {16, 112, 32, 32},  {80, 112, 32, 46},  {144, 113, 32, 39},
        {208, 114, 40, 44}, {272, 120, 32, 32}, {344, 120, 24, 31},
        {408, 120, 24, 32},
    };
    return d;
}

EffectData make_ataqueComunDorado_effect_data() {
    EffectData d;
    d.ms_per_frame = 70;
    d.loop = false;
    d.frames = {
        {24, 192, 16, 8},   {82, 184, 29, 23},  {144, 184, 32, 24},
        {201, 172, 47, 41}, {264, 176, 48, 47}, {326, 193, 49, 31},
    };
    return d;
}

EffectData make_ataqueFlechas_effect_data() {
    EffectData d;
    d.ms_per_frame = 60;
    d.loop = false;
    d.frames = {
        {42, 59, 5, 5},     {167, 44, 18, 24},  {292, 41, 36, 29},
        {416, 24, 42, 48},  {541, 22, 47, 53},  {666, 18, 61, 60},
        {23, 144, 67, 64},  {152, 144, 70, 65}, {278, 142, 76, 70},
        {404, 140, 81, 75}, {531, 139, 84, 79}, {659, 139, 87, 81},
        {22, 264, 84, 87},  {161, 264, 75, 85}, {287, 264, 79, 84},
        {412, 263, 83, 86}, {538, 263, 85, 82}, {668, 266, 80, 66},
        {28, 400, 76, 64},  {163, 403, 69, 57}, {295, 407, 65, 53},
        {439, 413, 47, 41},
    };
    return d;
}

std::vector<EffectDef> all_effect_defs() {
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
        {EffectKind::AtaqueFlechas, make_ataqueFlechas_effect_data,
         "efectos/ataqueFlechas.png"},
    };
}
