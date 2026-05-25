#ifndef COMMON_COMMAND_H
#define COMMON_COMMAND_H

#include <string>

class Command {
    private:
    std::string payload;

    public:
    Command() = default;
    explicit Command(std::string payload)
        : payload(std::move(payload)) {}

    const std::string& text() const noexcept { return payload; }
    std::string str() const { return payload; }

    bool empty() const noexcept { return payload.empty(); }
    bool operator==(const Command& other) const noexcept {
        return payload == other.payload;
    }

    bool is_disconnect() const noexcept { return payload == "__DISCONNECT__"; }
    bool is_shutdown() const noexcept { return payload == "__SHUTDOWN__"; }
};

#endif
