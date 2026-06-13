#include "ui/console.h"

bool Console::is_open() const { return open_; }

void Console::open() { open_ = true; }

void Console::close() {
    open_ = false;
    input_.clear();
}

void Console::append(const std::string& text) { input_ += text; }

void Console::backspace() {
    if (input_.empty()) return;
    input_.pop_back();
    while (!input_.empty() &&
           (static_cast<unsigned char>(input_.back()) & 0xC0) == 0x80) {
        input_.pop_back();
    }
}

std::string Console::take() {
    std::string line = input_;
    input_.clear();
    return line;
}

const std::string& Console::current() const { return input_; }
