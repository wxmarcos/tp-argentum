#ifndef CLIENT_RENDER_CHARACTER_ANIMATOR_H
#define CLIENT_RENDER_CHARACTER_ANIMATOR_H

#include <cstdint>

#include "common/protocol_defs.h"
#include "render/animation.h"

class CharacterAnimator {
    private:
    Animation anim;
    protocol::Direction last_dir;

    public:
    CharacterAnimator(int frame_count, int ms_per_frame);

    void update(uint32_t delta_ms, protocol::Direction dir, bool moved);

    int current_frame() const { return anim.current_frame(); }

    protocol::Direction current_dir() const { return last_dir; }
};

#endif