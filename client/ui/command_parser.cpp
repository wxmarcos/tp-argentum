#include "ui/command_parser.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <exception>
#include <sstream>

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

std::vector<std::string> CommandParser::tokenize(const std::string& s) const {
    std::vector<std::string> tokens;
    std::istringstream ss(s);
    std::string tok;
    while (ss >> tok) {
        tokens.push_back(tok);
    }
    return tokens;
}

std::optional<uint16_t> CommandParser::to_u16(const std::string& s) const {
    try {
        return static_cast<uint16_t>(std::stoul(s));
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::optional<uint32_t> CommandParser::to_u32(const std::string& s) const {
    try {
        return static_cast<uint32_t>(std::stoul(s));
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::optional<Command> CommandParser::parse_at(const std::string& rest) const {
    std::string nick;
    std::string msg;
    split_first(trim(rest), nick, msg);
    if (nick.empty() || msg.empty()) {
        return std::nullopt;
    }
    return Command::private_message(nick, msg);
}

std::optional<Command> CommandParser::parse_deposit(
    const std::string& rest) const {
    const auto args = tokenize(rest);
    if (args.size() == 2 && to_lower(args[0]) == "oro") {
        if (auto n = to_u32(args[1])) return Command::deposit_gold(*n);
        return std::nullopt;
    }
    if (args.size() == 1) {
        if (auto s = to_u16(args[0])) return Command::deposit_item(*s);
    }
    return std::nullopt;
}

std::optional<Command> CommandParser::parse_withdraw(
    const std::string& rest) const {
    const auto args = tokenize(rest);
    if (args.size() == 2 && to_lower(args[0]) == "oro") {
        if (auto n = to_u32(args[1])) return Command::withdraw_gold(*n);
        return std::nullopt;
    }
    if (args.size() == 1) {
        if (auto id = to_u16(args[0])) return Command::withdraw_item(*id);
    }
    return std::nullopt;
}

std::optional<Command> CommandParser::parse(const std::string& line) const {
    const std::string clean = trim(line);
    if (clean.empty()) {
        return std::nullopt;
    }
    if (clean[0] == '@') {
        return parse_at(clean.substr(1));
    }
    if (clean[0] != '/') {
        return std::nullopt;
    }

    std::string cmd;
    std::string rest;
    split_first(clean.substr(1), cmd, rest);
    cmd = to_lower(cmd);

    if (cmd == "meditar") return Command::meditate();
    if (cmd == "curar") return Command::heal();
    if (cmd == "resucitar") return Command::resurrect();
    if (cmd == "tomar") return Command::pick_item();
    if (cmd == "comprar") {
        if (rest.empty()) return std::nullopt;
        return Command::buy_item(rest);
    }
    if (cmd == "equipar") {
        if (auto s = to_u16(rest)) return Command::equip_item(*s);
        return std::nullopt;
    }
    if (cmd == "tirar") {
        if (auto s = to_u16(rest)) return Command::drop_item(*s);
        return std::nullopt;
    }
    if (cmd == "vender") {
        if (auto s = to_u16(rest)) return Command::sell_item(*s);
        return std::nullopt;
    }
    if (cmd == "depositar") return parse_deposit(rest);
    if (cmd == "retirar") return parse_withdraw(rest);

    return std::nullopt;
}
