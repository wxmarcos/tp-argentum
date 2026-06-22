#include "render/sprites/head_adjuster.h"

#include "game/entity_keys.h"

namespace {

// Tabla de ajustes de cabeza.
// Convencion: dy < 0 sube, dx < 0 va hacia la izquierda.
// Columnas:                          SUR        NORTE     OESTE     ESTE
constexpr HeadTweak kHeadTweaks[] = {
    {keys::HUMANO, keys::MAGO, {{-1, -2}, {-2, -3}, {0, -4}, {0, -4}}},
    {keys::HUMANO, keys::CLERIGO, {{-1, -2}, {-2, -3}, {0, -4}, {0, -4}}},
    {keys::HUMANO, keys::GUERRERO, {{-1, -2}, {-2, -3}, {0, -4}, {0, -4}}},
    {keys::HUMANO, keys::PALADIN, {{-2, -4}, {-2, -3}, {0, -4}, {0, -4}}},

    {keys::ELFO, keys::MAGO, {{0, -4}, {-1, -4}, {0, -4}, {0, -4}}},
    {keys::ELFO, keys::CLERIGO, {{1, -4}, {1, -4}, {2, -4}, {-2, -4}}},
    {keys::ELFO, keys::GUERRERO, {{1, -4}, {1, -4}, {1, -4}, {-1, -4}}},
    {keys::ELFO, keys::PALADIN, {{-1, -4}, {0, -4}, {1, -6}, {-1, -6}}},

    {keys::ENANO, keys::MAGO, {{0, 0}, {0, -2}, {0, 0}, {0, 0}}},
    {keys::ENANO, keys::CLERIGO, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},
    {keys::ENANO, keys::GUERRERO, {{0, 0}, {0, -1}, {0, 0}, {0, 0}}},
    {keys::ENANO, keys::PALADIN, {{0, 0}, {1, -3}, {0, -2}, {0, -2}}},

    {keys::GNOMO, keys::MAGO, {{-1, -4}, {-1, -7}, {0, -4}, {0, -4}}},
    {keys::GNOMO, keys::CLERIGO, {{-1, -4}, {0, -3}, {2, -3}, {-2, -3}}},
    {keys::GNOMO, keys::GUERRERO, {{0, -4}, {0, -4}, {0, -4}, {0, -4}}},
    {keys::GNOMO, keys::PALADIN, {{-1, -4}, {0, -4}, {1, -5}, {-1, -5}}},
};

// Lo mismo para ajustar los cascos. Mismas consideraciones.
constexpr HelmetTweak kHelmetTweaks[] = {
    {keys::HUMANO, keys::CASCO_CAPUCHA, {{0, 0}, {2, 0}, {-2, 0}, {3, 0}}, 100},
    {keys::HUMANO, keys::CASCO_HIERRO, {{1, 3}, {1, 0}, {-5, 2}, {5, 2}}, 120},
    {keys::HUMANO,
     keys::CASCO_SOMBRERO,
     {{0, 0}, {2, 0}, {-2, 0}, {2, 0}},
     100},

    {keys::ELFO,
     keys::CASCO_CAPUCHA,
     {{0, -1}, {0, -1}, {0, -1}, {0, -1}},
     100},
    {keys::ELFO, keys::CASCO_HIERRO, {{0, 3}, {0, 0}, {-5, 2}, {5, 2}}, 120},
    {keys::ELFO, keys::CASCO_SOMBRERO, {{0, 0}, {0, 2}, {-2, 0}, {2, 0}}, 100},

    {keys::ENANO, keys::CASCO_CAPUCHA, {{0, -1}, {0, 0}, {0, 0}, {0, 0}}, 100},
    {keys::ENANO, keys::CASCO_HIERRO, {{0, 0}, {-1, 0}, {-4, 0}, {2, 0}}, 100},
    {keys::ENANO,
     keys::CASCO_SOMBRERO,
     {{0, 0}, {0, 1}, {-1, -1}, {1, -1}},
     100},

    {keys::GNOMO, keys::CASCO_CAPUCHA, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}, 100},
    {keys::GNOMO, keys::CASCO_HIERRO, {{0, 0}, {0, 0}, {-3, 0}, {3, 0}}, 100},
    {keys::GNOMO, keys::CASCO_SOMBRERO, {{0, 2}, {0, 5}, {-1, 2}, {1, 2}}, 100},
};

}  // namespace

HeadAdjust HeadAdjuster::head(const std::string& raza,
                              const std::string& cuerpo, int dir_idx) {
    for (const auto& t : kHeadTweaks) {
        if (raza == t.raza && cuerpo == t.cuerpo) {
            return t.dir[dir_idx & 3];
        }
    }
    return {};
}

HeadAdjust HeadAdjuster::helmet(const std::string& raza,
                                const std::string& casco, int dir_idx) {
    for (const auto& t : kHelmetTweaks) {
        if (raza == t.raza && casco == t.casco) {
            return t.dir[dir_idx & 3];
        }
    }
    return {};
}

int HeadAdjuster::helmet_scale_pct(const std::string& raza,
                                   const std::string& casco) {
    for (const auto& t : kHelmetTweaks) {
        if (raza == t.raza && casco == t.casco) {
            return t.scale_pct;
        }
    }
    return 100;
}
