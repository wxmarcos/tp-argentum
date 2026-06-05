#ifndef CLIENT_RENDER_ANIMATION_H
#define CLIENT_RENDER_ANIMATION_H

#include <cstdint>

class Animation {
private:
    int frame_count;
    int ms_per_frame;
    int current;
    uint32_t elapsed_ms;

public:
    Animation(int frame_count, int ms_per_frame);

    void update(uint32_t delta_ms);

    void reset();

    int current_frame() const { return current; }
};

#endif
