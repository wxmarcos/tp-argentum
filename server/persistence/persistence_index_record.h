#pragma once

#include <cstdint>

constexpr std::size_t PERSISTENCE_INDEX_NICK_SIZE = 32;

struct PersistenceIndexRecord {
    char nick[PERSISTENCE_INDEX_NICK_SIZE];
    uint64_t offset;
};