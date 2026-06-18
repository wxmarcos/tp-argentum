#include "server/persistence/clan/clan_saver.h"

#include <fstream>
#include <iostream>

#include "server/persistence/clan/clan_record.h"
#include "server/persistence/clan/clan_record_mapper.h"

void ClanSaver::save_all(const std::string& path,
                         const std::map<std::string, Clan>& clanes) {
    std::ofstream file(path, std::ios::binary | std::ios::trunc);

    if (!file.is_open()) {
        std::cout << "[ClanSaver] no se pudo abrir " << path << "\n";
        return;
    }

    for (const auto& [nombre, clan] : clanes) {
        ClanRecord record = ClanRecordMapper::to_record(clan);

        file.write(reinterpret_cast<const char*>(&record),
                   sizeof(ClanRecord));
    }

    std::cout << "[ClanSaver] clanes guardados=" << clanes.size() << "\n";
}