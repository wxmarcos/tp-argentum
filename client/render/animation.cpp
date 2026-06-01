#include "render/animation.h"

Animation::Animation(int frame_count, int ms_per_frame):
        frame_count(frame_count),
        ms_per_frame(ms_per_frame),
        current(0),
        elapsed_ms(0) {}

void Animation::update(uint32_t delta_ms) {
    elapsed_ms += delta_ms;
    if (elapsed_ms >= static_cast<uint32_t>(ms_per_frame)) {
        elapsed_ms -= static_cast<uint32_t>(ms_per_frame);
        current = (current + 1) % frame_count;
    }
}

void Animation::reset() {
    current = 0;
    elapsed_ms = 0;
}
