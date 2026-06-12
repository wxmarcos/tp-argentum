#ifndef CLIENT_UI_COMMAND_PARSER_H
#define CLIENT_UI_COMMAND_PARSER_H

#include <optional>
#include <string>

#include "common/command/command.h"

class CommandParser {
    private:
    std::string trim(const std::string& s) const;

    std::string to_lower(const std::string& s) const;

    void split_first(const std::string& s, std::string& head,
                     std::string& tail) const;
                     
    std::optional<Command> parse_gold(const std::string& arg,
                                      bool deposit) const;

    public:
    std::optional<Command> parse(const std::string& line) const;
};

#endif