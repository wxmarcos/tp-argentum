#ifndef CLIENT_RENDER_SPRITE_DATA_H
#define CLIENT_RENDER_SPRITE_DATA_H
 
#include <SDL2/SDL.h>
 
static constexpr int WALK_FRAME_COUNT = 4;
static constexpr int DEFAULT_HUMAN_HEAD_NECK = 24;
 
static constexpr int DIR_SOUTH = 0;
static constexpr int DIR_NORTH = 1;
static constexpr int DIR_EAST  = 2;
static constexpr int DIR_WEST  = 3;
 
struct SpriteData {
    SDL_Rect frame_rects[4][WALK_FRAME_COUNT];
    int head_offset_y = 0;
    int head_neck = DEFAULT_HUMAN_HEAD_NECK;
}; 
 
inline SpriteData make_humano_sprite_data() {
    SpriteData d;
    d.frame_rects[DIR_SOUTH][0] = {   3,   5, 19, 38 };
    d.frame_rects[DIR_SOUTH][1] = {  29,   5, 19, 38 };
    d.frame_rects[DIR_SOUTH][2] = {  53,   5, 19, 38 };
    d.frame_rects[DIR_SOUTH][3] = {  76,   5, 19, 38 };
 
    d.frame_rects[DIR_NORTH][0] = {   3,  51, 19, 38 };
    d.frame_rects[DIR_NORTH][1] = {  29,  51, 19, 38 };
    d.frame_rects[DIR_NORTH][2] = {  53,  51, 19, 38 };
    d.frame_rects[DIR_NORTH][3] = {  76,  51, 19, 38 };
 
    d.frame_rects[DIR_EAST][0] = {   4,  95, 19, 38 };
    d.frame_rects[DIR_EAST][1] = {  28,  95, 19, 38 };
    d.frame_rects[DIR_EAST][2] = {  53,  95, 19, 38 };
    d.frame_rects[DIR_EAST][3] = {  78,  95, 19, 38 };
 
    d.frame_rects[DIR_WEST][0] = {   3, 140, 19, 38 };
    d.frame_rects[DIR_WEST][1] = {  29, 140, 19, 38 };
    d.frame_rects[DIR_WEST][2] = {  52, 140, 19, 38 };
    d.frame_rects[DIR_WEST][3] = {  76, 140, 19, 38 };
 
    d.head_neck = 24;
    return d;
}
 
inline SpriteData make_elfo_sprite_data() {
    SpriteData d;
    d.frame_rects[DIR_SOUTH][0] = {   3,  15, 19, 31 };
    d.frame_rects[DIR_SOUTH][1] = {  30,  15, 19, 31 };
    d.frame_rects[DIR_SOUTH][2] = {  57,  15, 19, 31 };
    d.frame_rects[DIR_SOUTH][3] = {  84,  15, 19, 31 };
 
    d.frame_rects[DIR_NORTH][0] = {   3,  61, 19, 31 };
    d.frame_rects[DIR_NORTH][1] = {  31,  61, 19, 31 };
    d.frame_rects[DIR_NORTH][2] = {  58,  61, 19, 31 };
    d.frame_rects[DIR_NORTH][3] = {  84,  61, 19, 31 };
 
    d.frame_rects[DIR_EAST][0] = {   5, 108, 19, 31 };
    d.frame_rects[DIR_EAST][1] = {  28, 108, 19, 31 };
    d.frame_rects[DIR_EAST][2] = {  56, 108, 19, 31 };
    d.frame_rects[DIR_EAST][3] = {  83, 108, 19, 31 };
 
    d.frame_rects[DIR_WEST][0] = {   2, 155, 19, 31 };
    d.frame_rects[DIR_WEST][1] = {  32, 155, 19, 31 };
    d.frame_rects[DIR_WEST][2] = {  59, 155, 19, 31 };
    d.frame_rects[DIR_WEST][3] = {  85, 155, 19, 31 };
 
    d.head_neck = 22;
    return d;
}
 
inline SpriteData make_enano_sprite_data() {
    SpriteData d;
    d.frame_rects[DIR_SOUTH][0] = {   5,  15, 17, 30 };
    d.frame_rects[DIR_SOUTH][1] = {  32,  15, 17, 30 };
    d.frame_rects[DIR_SOUTH][2] = {  60,  15, 17, 30 };
    d.frame_rects[DIR_SOUTH][3] = {  86,  15, 17, 30 };
 
    d.frame_rects[DIR_NORTH][0] = {   5,  62, 17, 30 };
    d.frame_rects[DIR_NORTH][1] = {  32,  62, 17, 30 };
    d.frame_rects[DIR_NORTH][2] = {  60,  62, 17, 30 };
    d.frame_rects[DIR_NORTH][3] = {  86,  62, 17, 30 };
 
    d.frame_rects[DIR_EAST][0] = {   4, 109, 17, 30 };
    d.frame_rects[DIR_EAST][1] = {  29, 109, 17, 30 };
    d.frame_rects[DIR_EAST][2] = {  57, 109, 17, 30 };
    d.frame_rects[DIR_EAST][3] = {  84, 109, 17, 30 };
 
    d.frame_rects[DIR_WEST][0] = {   6, 156, 17, 30 };
    d.frame_rects[DIR_WEST][1] = {  36, 156, 17, 30 };
    d.frame_rects[DIR_WEST][2] = {  61, 156, 17, 30 };
    d.frame_rects[DIR_WEST][3] = {  89, 156, 17, 30 };
 
    d.head_neck = 16;
    return d;
}
 
inline SpriteData make_gnomo_sprite_data() {
    SpriteData d;
    d.frame_rects[DIR_SOUTH][0] = {   5,  15, 17, 30 };
    d.frame_rects[DIR_SOUTH][1] = {  32,  15, 17, 30 };
    d.frame_rects[DIR_SOUTH][2] = {  59,  15, 17, 30 };
    d.frame_rects[DIR_SOUTH][3] = {  86,  15, 17, 30 };
 
    d.frame_rects[DIR_NORTH][0] = {   5,  63, 17, 30 };
    d.frame_rects[DIR_NORTH][1] = {  32,  63, 17, 30 };
    d.frame_rects[DIR_NORTH][2] = {  59,  63, 17, 30 };
    d.frame_rects[DIR_NORTH][3] = {  86,  63, 17, 30 };
 
    d.frame_rects[DIR_EAST][0] = {   5, 108, 17, 30 };
    d.frame_rects[DIR_EAST][1] = {  29, 108, 17, 30 };
    d.frame_rects[DIR_EAST][2] = {  57, 108, 17, 30 };
    d.frame_rects[DIR_EAST][3] = {  84, 108, 17, 30 };
 
    d.frame_rects[DIR_WEST][0] = {   6, 155, 17, 30 };
    d.frame_rects[DIR_WEST][1] = {  36, 155, 17, 30 };
    d.frame_rects[DIR_WEST][2] = {  62, 155, 17, 30 };
    d.frame_rects[DIR_WEST][3] = {  89, 155, 17, 30 };
 
    d.head_neck = 16;
    return d;
}
 
inline SpriteData make_mago_sprite_data() {
    SpriteData d;
    d.frame_rects[DIR_SOUTH][0] = {   4,  13, 20, 33 };
    d.frame_rects[DIR_SOUTH][1] = {  30,  13, 20, 33 };
    d.frame_rects[DIR_SOUTH][2] = {  58,  13, 20, 33 };
    d.frame_rects[DIR_SOUTH][3] = {  85,  13, 20, 33 };
 
    d.frame_rects[DIR_NORTH][0] = {   4,  61, 20, 33 };
    d.frame_rects[DIR_NORTH][1] = {  31,  61, 20, 33 };
    d.frame_rects[DIR_NORTH][2] = {  58,  61, 20, 33 };
    d.frame_rects[DIR_NORTH][3] = {  85,  61, 20, 33 };
 
    d.frame_rects[DIR_EAST][0] = {   3, 107, 20, 33 };
    d.frame_rects[DIR_EAST][1] = {  29, 107, 20, 33 };
    d.frame_rects[DIR_EAST][2] = {  56, 107, 20, 33 };
    d.frame_rects[DIR_EAST][3] = {  83, 107, 20, 33 };
 
    d.frame_rects[DIR_WEST][0] = {   4, 153, 20, 33 };
    d.frame_rects[DIR_WEST][1] = {  32, 153, 20, 33 };
    d.frame_rects[DIR_WEST][2] = {  59, 153, 20, 33 };
    d.frame_rects[DIR_WEST][3] = {  86, 153, 20, 33 };
 
    d.head_neck = 24;
    return d;
}
 
inline SpriteData make_clerigo_sprite_data() {
    SpriteData d;
    d.frame_rects[DIR_SOUTH][0] = {   3,   6, 21, 38 };
    d.frame_rects[DIR_SOUTH][1] = {  30,   6, 21, 38 };
    d.frame_rects[DIR_SOUTH][2] = {  57,   6, 21, 38 };
    d.frame_rects[DIR_SOUTH][3] = {  84,   6, 21, 38 };
 
    d.frame_rects[DIR_NORTH][0] = {   3,  52, 21, 38 };
    d.frame_rects[DIR_NORTH][1] = {  30,  52, 21, 38 };
    d.frame_rects[DIR_NORTH][2] = {  57,  52, 21, 38 };
    d.frame_rects[DIR_NORTH][3] = {  84,  52, 21, 38 };
 
    d.frame_rects[DIR_WEST][0] = {   5,  99, 21, 38 };
    d.frame_rects[DIR_WEST][1] = {  29,  99, 21, 38 };
    d.frame_rects[DIR_WEST][2] = {  56,  99, 21, 38 };
    d.frame_rects[DIR_WEST][3] = {  83,  99, 21, 38 };
 
    d.frame_rects[DIR_EAST][0] = {   2, 146, 21, 38 };
    d.frame_rects[DIR_EAST][1] = {  32, 146, 21, 38 };
    d.frame_rects[DIR_EAST][2] = {  58, 146, 21, 38 };
    d.frame_rects[DIR_EAST][3] = {  85, 146, 21, 38 };
 
    d.head_neck = 16;
    return d;
}
 
inline SpriteData make_guerrero_sprite_data() {
    SpriteData d;
    d.frame_rects[DIR_SOUTH][0] = {   4,   6, 19, 40 };
    d.frame_rects[DIR_SOUTH][1] = {  31,   6, 19, 40 };
    d.frame_rects[DIR_SOUTH][2] = {  58,   6, 19, 40 };
    d.frame_rects[DIR_SOUTH][3] = {  85,   6, 19, 40 };
 
    d.frame_rects[DIR_NORTH][0] = {   4,  53, 19, 40 };
    d.frame_rects[DIR_NORTH][1] = {  31,  53, 19, 40 };
    d.frame_rects[DIR_NORTH][2] = {  58,  53, 19, 40 };
    d.frame_rects[DIR_NORTH][3] = {  85,  53, 19, 40 };
 
    d.frame_rects[DIR_WEST][0] = {   5,  98, 19, 40 };
    d.frame_rects[DIR_WEST][1] = {  29,  98, 19, 40 };
    d.frame_rects[DIR_WEST][2] = {  56,  98, 19, 40 };
    d.frame_rects[DIR_WEST][3] = {  84,  98, 19, 40 };
 
    d.frame_rects[DIR_EAST][0] = {   4, 145, 19, 40 };
    d.frame_rects[DIR_EAST][1] = {  34, 145, 19, 40 };
    d.frame_rects[DIR_EAST][2] = {  60, 145, 19, 40 };
    d.frame_rects[DIR_EAST][3] = {  87, 145, 19, 40 };
 
    d.head_neck = 16;
    return d;
}
 
inline SpriteData make_paladin_sprite_data() {
    SpriteData d;
    d.frame_rects[DIR_SOUTH][0] = {   3,   5, 23, 38 };
    d.frame_rects[DIR_SOUTH][1] = {  30,   5, 23, 38 };
    d.frame_rects[DIR_SOUTH][2] = {  57,   5, 23, 38 };
    d.frame_rects[DIR_SOUTH][3] = {  84,   5, 23, 38 };
 
    d.frame_rects[DIR_NORTH][0] = {   2,  53, 23, 38 };
    d.frame_rects[DIR_NORTH][1] = {  29,  53, 23, 38 };
    d.frame_rects[DIR_NORTH][2] = {  56,  53, 23, 38 };
    d.frame_rects[DIR_NORTH][3] = {  83,  53, 23, 38 };
 
    d.frame_rects[DIR_WEST][0] = {   2, 101, 23, 38 };
    d.frame_rects[DIR_WEST][1] = {  28, 101, 23, 38 };
    d.frame_rects[DIR_WEST][2] = {  55, 101, 23, 38 };
    d.frame_rects[DIR_WEST][3] = {  82, 101, 23, 38 };
 
    d.frame_rects[DIR_EAST][0] = {   3, 148, 23, 38 };
    d.frame_rects[DIR_EAST][1] = {  31, 148, 23, 38 };
    d.frame_rects[DIR_EAST][2] = {  58, 148, 23, 38 };
    d.frame_rects[DIR_EAST][3] = {  85, 148, 23, 38 };
 
    d.head_neck = 18;
    return d;
}
 
inline SpriteData make_goblin_sprite_data() {
    SpriteData d;
    d.frame_rects[DIR_SOUTH][0] = {   3,   6,  18,  25 };
    d.frame_rects[DIR_SOUTH][1] = {  28,   6,  18,  25 };
    d.frame_rects[DIR_SOUTH][2] = {  53,   6,  18,  25 };
    d.frame_rects[DIR_SOUTH][3] = {  78,   6,  18,  25 };

    d.frame_rects[DIR_NORTH][0] = {   3,  36,  18,  25 };
    d.frame_rects[DIR_NORTH][1] = {  28,  36,  18,  25 };
    d.frame_rects[DIR_NORTH][2] = {  53,  36,  18,  25 };
    d.frame_rects[DIR_NORTH][3] = {  78,  36,  18,  25 };

    d.frame_rects[DIR_EAST][0] = {   3,  67,  18,  25 };
    d.frame_rects[DIR_EAST][1] = {  28,  67,  18,  25 };
    d.frame_rects[DIR_EAST][2] = {  53,  67,  18,  25 };
    d.frame_rects[DIR_EAST][3] = {  78,  67,  18,  25 };

    d.frame_rects[DIR_WEST][0] = {   4,  98,  18,  25 };
    d.frame_rects[DIR_WEST][1] = {  30,  98,  18,  25 };
    d.frame_rects[DIR_WEST][2] = {  54,  98,  18,  25 };
    d.frame_rects[DIR_WEST][3] = {  79,  98,  18,  25 };

    return d;
}

inline SpriteData make_esqueleto_sprite_data() {
    SpriteData d;
    d.frame_rects[DIR_SOUTH][0] = {   4,   2,  21,  45 };
    d.frame_rects[DIR_SOUTH][1] = {  29,   2,  21,  45 };
    d.frame_rects[DIR_SOUTH][2] = {  53,   2,  21,  45 };
    d.frame_rects[DIR_SOUTH][3] = {  80,   2,  21,  45 };

    d.frame_rects[DIR_NORTH][0] = {   1,  49,  21,  45 };
    d.frame_rects[DIR_NORTH][1] = {  26,  49,  21,  45 };
    d.frame_rects[DIR_NORTH][2] = {  51,  49,  21,  45 };
    d.frame_rects[DIR_NORTH][3] = {  75,  49,  21,  45 };

    d.frame_rects[DIR_EAST][0] = {   3, 143,  21,  45 };
    d.frame_rects[DIR_EAST][1] = {  29, 143,  21,  45 };
    d.frame_rects[DIR_EAST][2] = {  53, 143,  21,  45 };
    d.frame_rects[DIR_EAST][3] = {  77, 143,  21,  45 };

    d.frame_rects[DIR_WEST][0] = {   3,  96,  21,  45 };
    d.frame_rects[DIR_WEST][1] = {  28,  96,  21,  45 };
    d.frame_rects[DIR_WEST][2] = {  53,  96,  21,  45 };
    d.frame_rects[DIR_WEST][3] = {  77,  96,  21,  45 };

    return d;
}

inline SpriteData make_zombie_sprite_data() {
    SpriteData d;
    d.frame_rects[DIR_SOUTH][0] = {   6,   2,  15,  43 };
    d.frame_rects[DIR_SOUTH][1] = {  33,   2,  15,  43 };
    d.frame_rects[DIR_SOUTH][2] = {  61,   2,  15,  43 };
    d.frame_rects[DIR_SOUTH][3] = {  88,   2,  15,  43 };

    d.frame_rects[DIR_NORTH][0] = {   5,  49,  15,  43 };
    d.frame_rects[DIR_NORTH][1] = {  32,  49,  15,  43 };
    d.frame_rects[DIR_NORTH][2] = {  60,  49,  15,  43 };
    d.frame_rects[DIR_NORTH][3] = {  87,  49,  15,  43 };

    d.frame_rects[DIR_EAST][0] = {   6,  95,  15,  43 };
    d.frame_rects[DIR_EAST][1] = {  33,  95,  15,  43 };
    d.frame_rects[DIR_EAST][2] = {  61,  95,  15,  43 };
    d.frame_rects[DIR_EAST][3] = {  88,  95,  15,  43 };

    d.frame_rects[DIR_WEST][0] = {   4, 142,  15,  43 };
    d.frame_rects[DIR_WEST][1] = {  31, 142,  15,  43 };
    d.frame_rects[DIR_WEST][2] = {  59, 142,  15,  43 };
    d.frame_rects[DIR_WEST][3] = {  86, 142,  15,  43 };

    return d;
}

inline SpriteData make_orco_sprite_data() {
    SpriteData d;
    d.frame_rects[DIR_SOUTH][0] = {   4,   5,  53,  87 };
    d.frame_rects[DIR_SOUTH][1] = {  60,   5,  53,  87 };
    d.frame_rects[DIR_SOUTH][2] = { 118,   5,  53,  87 };
    d.frame_rects[DIR_SOUTH][3] = { 174,   5,  53,  87 };

    d.frame_rects[DIR_NORTH][0] = {   4, 101,  53,  87 };
    d.frame_rects[DIR_NORTH][1] = {  61, 101,  53,  87 };
    d.frame_rects[DIR_NORTH][2] = { 117, 101,  53,  87 };
    d.frame_rects[DIR_NORTH][3] = { 174, 101,  53,  87 };

    d.frame_rects[DIR_EAST][0] = {   4, 201,  53,  87 };
    d.frame_rects[DIR_EAST][1] = {  60, 201,  53,  87 };
    d.frame_rects[DIR_EAST][2] = { 117, 201,  53,  87 };
    d.frame_rects[DIR_EAST][3] = { 174, 201,  53,  87 };

    d.frame_rects[DIR_WEST][0] = {   6, 300,  53,  87 };
    d.frame_rects[DIR_WEST][1] = {  60, 300,  53,  87 };
    d.frame_rects[DIR_WEST][2] = { 118, 300,  53,  87 };
    d.frame_rects[DIR_WEST][3] = { 175, 300,  53,  87 };

    return d;
}

inline SpriteData make_arana_sprite_data() {
    SpriteData d;
    d.frame_rects[DIR_SOUTH][0] = {   5,  38,  88,  57 };
    d.frame_rects[DIR_SOUTH][1] = { 103,  38,  88,  57 };
    d.frame_rects[DIR_SOUTH][2] = { 198,  38,  88,  57 };
    d.frame_rects[DIR_SOUTH][3] = { 292,  38,  88,  57 };

    d.frame_rects[DIR_NORTH][0] = {   4, 135,  88,  57 };
    d.frame_rects[DIR_NORTH][1] = { 100, 135,  88,  57 };
    d.frame_rects[DIR_NORTH][2] = { 197, 135,  88,  57 };
    d.frame_rects[DIR_NORTH][3] = { 295, 135,  88,  57 };

    d.frame_rects[DIR_EAST][0] = {   3, 230,  88,  57 };
    d.frame_rects[DIR_EAST][1] = {  98, 230,  88,  57 };
    d.frame_rects[DIR_EAST][2] = { 195, 230,  88,  57 };
    d.frame_rects[DIR_EAST][3] = { 291, 230,  88,  57 };

    d.frame_rects[DIR_WEST][0] = {   7, 326,  88,  57 };
    d.frame_rects[DIR_WEST][1] = { 104, 326,  88,  57 };
    d.frame_rects[DIR_WEST][2] = { 199, 326,  88,  57 };
    d.frame_rects[DIR_WEST][3] = { 296, 326,  88,  57 };

    return d;
}

inline SpriteData make_golem_sprite_data() {
    SpriteData d;
    d.frame_rects[DIR_SOUTH][0] = {  11,   5,  73,  80 };
    d.frame_rects[DIR_SOUTH][1] = { 107,   5,  73,  80 };
    d.frame_rects[DIR_SOUTH][2] = { 203,   5,  73,  80 };
    d.frame_rects[DIR_SOUTH][3] = { 299,   5,  73,  80 };

    d.frame_rects[DIR_NORTH][0] = {  12, 104,  73,  80 };
    d.frame_rects[DIR_NORTH][1] = { 108, 104,  73,  80 };
    d.frame_rects[DIR_NORTH][2] = { 204, 104,  73,  80 };
    d.frame_rects[DIR_NORTH][3] = { 300, 104,  73,  80 };

    d.frame_rects[DIR_EAST][0] = {   8, 297,  73,  80 };
    d.frame_rects[DIR_EAST][1] = { 104, 297,  73,  80 };
    d.frame_rects[DIR_EAST][2] = { 200, 297,  73,  80 };
    d.frame_rects[DIR_EAST][3] = { 296, 297,  73,  80 };

    d.frame_rects[DIR_WEST][0] = {  16, 203,  73,  80 };
    d.frame_rects[DIR_WEST][1] = { 111, 203,  73,  80 };
    d.frame_rects[DIR_WEST][2] = { 208, 203,  73,  80 };
    d.frame_rects[DIR_WEST][3] = { 303, 203,  73,  80 };

    return d;
}

inline SpriteData make_banquero_sprite_data() {
    SpriteData d;
    d.frame_rects[DIR_SOUTH][0] = {   3,   1,  19,  47 };
    d.frame_rects[DIR_SOUTH][1] = {  26,   1,  19,  47 };
    d.frame_rects[DIR_SOUTH][2] = {  49,   1,  19,  47 };
    d.frame_rects[DIR_SOUTH][3] = {  72,   1,  19,  47 };

    d.frame_rects[DIR_NORTH][0] = {   3,  47,  19,  47 };
    d.frame_rects[DIR_NORTH][1] = {  26,  47,  19,  47 };
    d.frame_rects[DIR_NORTH][2] = {  49,  47,  19,  47 };
    d.frame_rects[DIR_NORTH][3] = {  72,  47,  19,  47 };

    d.frame_rects[DIR_EAST][0] = {   4,  94,  19,  47 };
    d.frame_rects[DIR_EAST][1] = {  26,  94,  19,  47 };
    d.frame_rects[DIR_EAST][2] = {  48,  94,  19,  47 };
    d.frame_rects[DIR_EAST][3] = {  71,  94,  19,  47 };

    d.frame_rects[DIR_WEST][0] = {   4, 141,  19,  47 };
    d.frame_rects[DIR_WEST][1] = {  27, 141,  19,  47 };
    d.frame_rects[DIR_WEST][2] = {  52, 141,  19,  47 };
    d.frame_rects[DIR_WEST][3] = {  75, 141,  19,  47 };

    return d;
}

inline SpriteData make_comerciante_sprite_data() {
    SpriteData d;
    d.frame_rects[DIR_SOUTH][0] = {   3,   1,  21,  43 };
    d.frame_rects[DIR_SOUTH][1] = {  31,   1,  21,  43 };
    d.frame_rects[DIR_SOUTH][2] = {  58,   1,  21,  43 };
    d.frame_rects[DIR_SOUTH][3] = {  84,   1,  21,  43 };

    d.frame_rects[DIR_NORTH][0] = {   3,  48,  21,  43 };
    d.frame_rects[DIR_NORTH][1] = {  30,  48,  21,  43 };
    d.frame_rects[DIR_NORTH][2] = {  57,  48,  21,  43 };
    d.frame_rects[DIR_NORTH][3] = {  84,  48,  21,  43 };

    d.frame_rects[DIR_EAST][0] = {   5,  94,  21,  43 };
    d.frame_rects[DIR_EAST][1] = {  31,  94,  21,  43 };
    d.frame_rects[DIR_EAST][2] = {  59,  94,  21,  43 };
    d.frame_rects[DIR_EAST][3] = {  86,  94,  21,  43 };

    d.frame_rects[DIR_WEST][0] = {   3, 141,  21,  43 };
    d.frame_rects[DIR_WEST][1] = {  31, 141,  21,  43 };
    d.frame_rects[DIR_WEST][2] = {  57, 141,  21,  43 };
    d.frame_rects[DIR_WEST][3] = {  84, 141,  21,  43 };

    return d;
}

inline SpriteData make_sacerdote_sprite_data() {
    SpriteData d;
    d.frame_rects[DIR_SOUTH][0] = {   4,   0,  17,  45 };
    d.frame_rects[DIR_SOUTH][1] = {  29,   0,  17,  45 };
    d.frame_rects[DIR_SOUTH][2] = {  54,   0,  17,  45 };
    d.frame_rects[DIR_SOUTH][3] = {  79,   0,  17,  45 };

    d.frame_rects[DIR_NORTH][0] = {   4,  45,  17,  45 };
    d.frame_rects[DIR_NORTH][1] = {  29,  45,  17,  45 };
    d.frame_rects[DIR_NORTH][2] = {  54,  45,  17,  45 };
    d.frame_rects[DIR_NORTH][3] = {  79,  45,  17,  45 };

    d.frame_rects[DIR_EAST][0] = {   4,  90,  17,  45 };
    d.frame_rects[DIR_EAST][1] = {  28,  90,  17,  45 };
    d.frame_rects[DIR_EAST][2] = {  54,  90,  17,  45 };
    d.frame_rects[DIR_EAST][3] = {  79,  90,  17,  45 };

    d.frame_rects[DIR_WEST][0] = {   6, 135,  17,  45 };
    d.frame_rects[DIR_WEST][1] = {  33, 135,  17,  45 };
    d.frame_rects[DIR_WEST][2] = {  55, 135,  17,  45 };
    d.frame_rects[DIR_WEST][3] = {  79, 135,  17,  45 };

    return d;
}

inline SpriteData make_standard_sprite_data() {
    return make_humano_sprite_data();
}
 
struct HeadData {
    SDL_Rect head_rects[4];
};
 
inline HeadData make_humano_head_data() {
    HeadData d;
    d.head_rects[DIR_SOUTH] = {  6,  13, 13, 17 };
    d.head_rects[DIR_NORTH] = {  6,  77, 13, 16 };
    d.head_rects[DIR_EAST]  = {  6, 205, 13, 17 };
    d.head_rects[DIR_WEST]  = {  6, 141, 13, 17 };
    return d;
}
 
inline HeadData make_elfo_head_data() {
    HeadData d;
    d.head_rects[DIR_SOUTH] = {  6,  13, 15, 16 };
    d.head_rects[DIR_NORTH] = {  6,  77, 15, 16 };
    d.head_rects[DIR_EAST]  = {  6, 205, 15, 16 };
    d.head_rects[DIR_WEST]  = {  6, 141, 15, 16 };
    return d;
}
 
inline HeadData make_enano_head_data() {
    HeadData d;
    d.head_rects[DIR_SOUTH] = {  7,  16, 13, 22 };
    d.head_rects[DIR_NORTH] = {  7,  80, 13, 15 };
    d.head_rects[DIR_EAST]  = {  7, 208, 13, 22 };
    d.head_rects[DIR_WEST]  = {  7, 144, 13, 22 };
    return d;
}
 
inline HeadData make_gnomo_head_data() {
    HeadData d;
    d.head_rects[DIR_SOUTH] = {  7,  14, 12, 18 };
    d.head_rects[DIR_NORTH] = {  7,  76, 12, 15 };
    d.head_rects[DIR_EAST]  = {  7, 205, 12, 18 };
    d.head_rects[DIR_WEST]  = {  7, 141, 12, 18 };
    return d;
}
 
#endif