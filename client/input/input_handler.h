#ifndef CLIENT_INPUT_HANDLER_H
#define CLIENT_INPUT_HANDLER_H

#include <unordered_map>
#include <SDL2/SDL.h>

#include "input/key_action.h"
#include "protocol/client_command.h"

class InputHandler {
    private:
    std::unordered_map<SDL_Keycode, KeyAction> bindings;

    void load_default_bindings();

    bool action_to_command(KeyAction action, ClientCommand& out_cmd) const;

    public:
    InputHandler();

    bool process_key(const SDL_KeyboardEvent& key, ClientCommand& out_cmd) const;
};

#endif