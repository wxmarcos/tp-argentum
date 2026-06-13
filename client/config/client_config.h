#ifndef CLIENT_CONFIG_H
#define CLIENT_CONFIG_H

#include <map>
#include <string>

struct ClientConfig {
    std::string server_host = "localhost";
    std::string server_port = "8080";

    int window_width = 1280;
    int window_height = 720;
    bool fullscreen = false;
    double hud_panel_fraction = 0.28;
    std::string window_title = "Argentum Online";

    int tile_size = 32;

    std::string assets_path = "assets/graficos";
    std::string font_path = "assets/fonts/Cardo-Bold.ttf";
    int font_size = 18;

    int map_width = 50;
    int map_height = 50;
    std::string map_name = "bosqueOscuro";
    std::map<int, std::string> map_files;

    int music_volume = 64;
    int effects_volume = 96;

    std::string character_nick = "Messi";
    std::string character_raza = "humano";
    std::string character_clase = "mago";

    std::string map_name_for(int id) const;

    int hud_panel_width() const;

    int game_area_width() const;
    
    static ClientConfig load();

    private:
    static std::string find_config_file();

    template <typename T, typename Table>
    static void read_field(const Table& tbl, const std::string& section,
                           const std::string& key, T& out);

    template <typename Table>
    void apply_table(const Table& tbl);

    template <typename Table>
    void read_map_files(const Table& tbl);
};

#endif