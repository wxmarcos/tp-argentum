#include "render/character_animator.h"

#include <SDL2/SDL.h>

static constexpr uint32_t MOVE_ANIM_DURATION_MS = 400;

CharacterAnimator::CharacterAnimator(int frame_count, int ms_per_frame):
    anim(frame_count, ms_per_frame), last_dir(protocol::Direction::SOUTH),
    moving_ms_remaining(0) {}

void CharacterAnimator::update(uint32_t delta_ms, protocol::Direction dir,
                               bool moved) {
    if (dir != last_dir) {
        anim.reset();
        last_dir = dir;
    }

    if (moved) {
        moving_ms_remaining = MOVE_ANIM_DURATION_MS;
    }

    if (moving_ms_remaining > 0) {
        anim.update(delta_ms);
        if (delta_ms >= moving_ms_remaining) {
            moving_ms_remaining = 0;
            anim.reset();
        } else {
            moving_ms_remaining -= delta_ms;
        }
    }
}
