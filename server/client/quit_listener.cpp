#include "quit_listener.h"

#include <iostream>

QuitListener::QuitListener(std::atomic<bool>& running)
    : running(running) {}

void QuitListener::run() {
    char c;
    
    while (should_keep_running() && running && std::cin >> c) {
        if (c == 'q' || c == 'Q') {
            running = false;
            break;
        }
    }
}