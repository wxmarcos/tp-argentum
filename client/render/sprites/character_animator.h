#ifndef CLIENT_RENDER_CHARACTER_ANIMATOR_H
#define CLIENT_RENDER_CHARACTER_ANIMATOR_H

#include <cstdint>

#include "common/protocol_defs.h"
#include "render/sprites/animation.h"

class CharacterAnimator {
private:
    Animation anim;
    protocol::Direction last_dir;
    uint32_t moving_ms_remaining;

public:
    CharacterAnimator(int frame_count, int ms_per_frame);

    void update(uint32_t delta_ms, protocol::Direction dir, bool moved);

    int current_frame() const;

    protocol::Direction current_dir() const;
};

#endif
