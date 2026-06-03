#ifndef CLIENT_CONFIG_H
#define CLIENT_CONFIG_H

#include <string>

struct ClientConfig {
    std::string server_host = "localhost";
    std::string server_port = "8080";

    int window_width = 800;
    int window_height = 600;
    bool fullscreen = false;
    std::string window_title = "Argentum Online";

    int tile_size = 32;

    std::string assets_path = "assets/graficos";

    int map_width = 50;
    int map_height = 50;

    int music_volume = 64;
    int effects_volume = 96;

    std::string character_nick = "Messi";
    std::string character_raza = "humano";
    std::string character_clase = "mago";

    static ClientConfig load();
};

#endif