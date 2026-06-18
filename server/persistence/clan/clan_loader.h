#pragma once

#include <string>
#include <unordered_map>
#include <map>
#include "game/clan.h"

class ClanLoader {
public:
    static std::map<std::string, Clan> load_all(const std::string& path);
};