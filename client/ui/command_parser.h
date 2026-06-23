#ifndef CLIENT_UI_COMMAND_PARSER_H
#define CLIENT_UI_COMMAND_PARSER_H

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "common/command/command.h"

class CommandParser {
private:
    std::string trim(const std::string& s) const;

    std::string to_lower(const std::string& s) const;

    void split_first(const std::string& s, std::string& head,
                     std::string& tail) const;

    std::vector<std::string> tokenize(const std::string& s) const;

    std::optional<uint16_t> to_u16(const std::string& s) const;

    std::optional<uint32_t> to_u32(const std::string& s) const;

    std::optional<Command> parse_at(const std::string& rest) const;

    std::optional<Command> parse_deposit(const std::string& rest) const;

    std::optional<Command> parse_withdraw(const std::string& rest) const;

public:
    std::optional<Command> parse(const std::string& line) const;
};

#endif
