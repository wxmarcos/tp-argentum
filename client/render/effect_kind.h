#ifndef CLIENT_RENDER_EFFECT_KIND_H
#define CLIENT_RENDER_EFFECT_KIND_H

#include <cstdint>

enum class EffectKind {
    Meditar,
    EfectoMorir,
    Resucitar,
    ExplosionSuper,
    ExplosionComun,
    Curarse,
    AtaqueBaculoComun,
    AtaqueBaculoDorado,
    AtaqueComunRojo,
    AtaqueComunGris,
    AtaqueComunDorado
};

struct EffectSpawn {
    uint16_t x = 0;
    uint16_t y = 0;
    EffectKind kind = EffectKind::EfectoMorir;
};

#endif