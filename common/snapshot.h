#ifndef COMMON_SNAPSHOT_H
#define COMMON_SNAPSHOT_H

#include <string>

class Snapshot {
    private:
    std::string payload;

    public:
    Snapshot() = default;
    explicit Snapshot(std::string payload)
        : payload(std::move(payload)) {}

    const std::string& text() const noexcept { return payload; }
    std::string str() const { return payload; }

    bool empty() const noexcept { return payload.empty(); }
    bool operator==(const Snapshot& other) const noexcept {
        return payload == other.payload;
    }

    bool is_disconnect() const noexcept { return payload == "__DISCONNECT__"; }
};

#endif
