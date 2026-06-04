#ifndef QUIT_LISTENER_H_
#define QUIT_LISTENER_H_

#include <atomic>

#include "common/thread.h"

class QuitListener : public Thread {
private:
    std::atomic<bool>& running;

public:
    explicit QuitListener(std::atomic<bool>& running);

    void run() override;
};

#endif