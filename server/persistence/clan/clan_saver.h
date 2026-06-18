#pragma once

#include <map>
#include <string>

#include "game/clan.h"

class ClanSaver {
public:
    static void save_all(const std::string& path,
                         const std::map<std::string, Clan>& clanes);
};