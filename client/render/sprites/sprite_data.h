#ifndef CLIENT_RENDER_SPRITE_DATA_H
#define CLIENT_RENDER_SPRITE_DATA_H

#include <SDL2/SDL.h>

#include "render/sprites/sprite_constants.h"

struct SpriteData {
    SDL_Rect frame_rects[4][WALK_FRAME_COUNT];
    int head_offset_y = 0;
    int head_neck = DEFAULT_HUMAN_HEAD_NECK;
};

struct HeadData {
    SDL_Rect head_rects[4];
    int off_x[4] = {0, 0, 0, 0};
    int off_y[4] = {0, 0, 0, 0};
    int scale_pct = 100;
};

SpriteData make_humano_sprite_data();
SpriteData make_elfo_sprite_data();
SpriteData make_enano_sprite_data();
SpriteData make_gnomo_sprite_data();
SpriteData make_fantasma_sprite_data();
SpriteData make_mago_sprite_data();
SpriteData make_clerigo_sprite_data();
SpriteData make_guerrero_sprite_data();
SpriteData make_paladin_sprite_data();
SpriteData make_goblin_sprite_data();
SpriteData make_esqueleto_sprite_data();
SpriteData make_zombie_sprite_data();
SpriteData make_orco_sprite_data();
SpriteData make_arana_sprite_data();
SpriteData make_golem_sprite_data();
SpriteData make_banquero_sprite_data();
SpriteData make_comerciante_sprite_data();
SpriteData make_sacerdote_sprite_data();
SpriteData make_standard_sprite_data();

HeadData make_humano_head_data();
HeadData make_elfo_head_data();
HeadData make_enano_head_data();
HeadData make_gnomo_head_data();

SpriteData make_armadura_cuero_sprite_data();
SpriteData make_armadura_placas_sprite_data();
SpriteData make_tunica_azul_sprite_data();

HeadData make_casco_capucha_data();
HeadData make_casco_hierro_data();
HeadData make_casco_sombrero_data();

#endif