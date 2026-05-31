#include "render/character_animator.h"

CharacterAnimator::CharacterAnimator(int frame_count, int ms_per_frame):
        anim(frame_count, ms_per_frame),
        last_dir(protocol::Direction::SOUTH) {}

void CharacterAnimator::update(uint32_t delta_ms,
                               protocol::Direction dir,
                               bool moved) {
    if (dir != last_dir) {
        anim.reset();
        last_dir = dir;
    }

    if (moved) {
        anim.update(delta_ms);
    } else {
        anim.reset();
    }
}
