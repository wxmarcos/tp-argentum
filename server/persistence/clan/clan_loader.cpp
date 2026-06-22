#include "server/persistence/clan/clan_loader.h"

#include <fstream>
#include <iostream>
#include <map>
#include "server/persistence/clan/clan_record.h"
#include "server/persistence/clan/clan_record_mapper.h"

std::map<std::string, Clan> ClanLoader::load_all(const std::string& path) {
    std::map<std::string, Clan> clanes;

    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        return clanes;
    }

    ClanRecord record{};

    while (file.read(reinterpret_cast<char*>(&record), sizeof(ClanRecord))) {
        Clan clan = ClanRecordMapper::from_record(record);

        if (!clan.getNombre().empty()) {
            clanes.emplace(clan.getNombre(), std::move(clan));
        }
    }

    return clanes;
}