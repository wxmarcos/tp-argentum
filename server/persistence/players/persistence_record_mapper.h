#pragma once

#include "server/persistence/players/persistence_record.h"
#include "server/persistence/persistence_task.h"

class PersistenceRecordMapper {
public:
    static PersistencePlayerRecord to_record(const PersistenceTask& task);
    static PersistenceTask from_record(const PersistencePlayerRecord& record);
};
