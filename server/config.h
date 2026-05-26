#pragma once

#include <string>

class Config {

public:

    static void load(const std::string& filename);

    // ===== SERVER =====
    static int MAP_WIDTH;
    static int MAP_HEIGHT;

    static int TICKS_PER_SECOND;

    static int MAX_CLIENTS;
};