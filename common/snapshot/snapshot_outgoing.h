#pragma once

#include <cstdint>
#include <utility>
#include <vector>

#include "common/snapshot/snapshot.h"

enum class DeliveryType {
    BROADCAST,
    UNICAST,
    MULTICAST
};

struct OutgoingSnapshot {
    Snapshot snapshot;
    DeliveryType delivery;
    std::vector<uint16_t> recipients;

    static OutgoingSnapshot broadcast(Snapshot snapshot) {
        return {std::move(snapshot), DeliveryType::BROADCAST, {}};
    }

    static OutgoingSnapshot unicast(Snapshot snapshot, uint16_t player_id) {
        return {std::move(snapshot), DeliveryType::UNICAST, {player_id}};
    }

    static OutgoingSnapshot multicast(Snapshot snapshot,
                                      std::vector<uint16_t> recipients) {
        return {std::move(snapshot), DeliveryType::MULTICAST,
                std::move(recipients)};
    }
};