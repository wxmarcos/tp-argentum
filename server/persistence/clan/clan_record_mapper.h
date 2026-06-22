#pragma once

#include "game/clan.h"
#include "server/persistence/clan/clan_record.h"

class ClanRecordMapper {
public:
    static ClanRecord to_record(const Clan& clan);
    static Clan from_record(const ClanRecord& record);
};
