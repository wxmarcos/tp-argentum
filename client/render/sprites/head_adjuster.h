#ifndef CLIENT_RENDER_HEAD_ADJUSTER_H
#define CLIENT_RENDER_HEAD_ADJUSTER_H

#include <string>
#include <string_view>

struct HeadAdjust {
    int dx = 0;
    int dy = 0;
};

struct HeadTweak {
    std::string_view raza;
    std::string_view cuerpo;
    HeadAdjust dir[4];
};

struct HelmetTweak {
    std::string_view raza;
    std::string_view casco;
    HeadAdjust dir[4];
    int scale_pct = 100;
};

class HeadAdjuster {
public:
    static HeadAdjust head(const std::string& raza, const std::string& cuerpo,
                           int dir_idx);

    static HeadAdjust helmet(const std::string& raza, const std::string& casco,
                             int dir_idx);

    static int helmet_scale_pct(const std::string& raza,
                                const std::string& casco);
};

#endif
