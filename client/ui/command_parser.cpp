#include "ui/command_parser.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <exception>

#include "common/protocol_defs.h"

std::string CommandParser::trim(const std::string& s) const {
    const auto begin = s.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) return {};
    const auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(begin, end - begin + 1);
}

std::string CommandParser::to_lower(const std::string& s) const {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return out;
}

void CommandParser::split_first(const std::string& s, std::string& head,
                                std::string& tail) const {
    const auto pos = s.find(' ');
    if (pos == std::string::npos) {
        head = s;
        tail = {};
        return;
    }
    head = s.substr(0, pos);
    tail = trim(s.substr(pos + 1));
}

std::optional<Command> CommandParser::parse_gold(const std::string& arg,
                                                 bool deposit) const {
    try {
        const uint32_t n = static_cast<uint32_t>(std::stoul(arg));
        return deposit ? Command::deposit_gold(n) : Command::withdraw_gold(n);
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::optional<Command> CommandParser::parse(const std::string& line) const {
    const std::string clean = trim(line);
    if (clean.empty() || clean[0] != '/') {
        return std::nullopt;
    }

    std::string cmd;
    std::string arg;
    split_first(clean.substr(1), cmd, arg);
    cmd = to_lower(cmd);

    if (cmd == "meditar") return Command(0, protocol::ClientOpcode::MEDITATE);
    if (cmd == "curar") return Command(0, protocol::ClientOpcode::HEAL);
    if (cmd == "resucitar")
        return Command(0, protocol::ClientOpcode::RESURRECT);
    if (cmd == "tomar") return Command(0, protocol::ClientOpcode::PICK_ITEM);
    if (cmd == "comprar" && !arg.empty()) return Command::buy_item(arg);
    if (cmd == "depositar") return parse_gold(arg, true);
    if (cmd == "retirar") return parse_gold(arg, false);

    return std::nullopt;
}
