#ifndef CLIENT_UI_CONSOLE_H
#define CLIENT_UI_CONSOLE_H

#include <string>

class Console {
    private:
    bool open_ = false;
    std::string input_;

    public:
    bool is_open() const;

    void open();
    
    void close();

    void append(const std::string& text);

    void backspace();

    std::string take();

    const std::string& current() const;
};

#endif