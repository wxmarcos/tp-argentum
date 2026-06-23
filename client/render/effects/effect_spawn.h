#ifndef CLIENT_RENDER_EFFECT_SPAWN_H
#define CLIENT_RENDER_EFFECT_SPAWN_H

#include <cstdint>

#include "render/effects/effect_kind.h"

struct EffectSpawn {
    uint16_t x = 0;
    uint16_t y = 0;
    EffectKind kind = EffectKind::EfectoMorir;
};

#endif
