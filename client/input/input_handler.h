#ifndef CLIENT_INPUT_HANDLER_H
#define CLIENT_INPUT_HANDLER_H

#include <unordered_map>
#include <SDL2/SDL.h>

#include "common/command/command.h"
#include "common/protocol_defs.h"
#include "input/key_action.h"

class InputHandler {
    private:
    std::unordered_map<SDL_Keycode, KeyAction> bindings;
    std::unordered_map<KeyAction, protocol::Direction> move_directions;

    void load_default_bindings();
    void load_move_directions();

    bool action_to_command(KeyAction action, Command& out_cmd) const;

    public:
    InputHandler();

    bool process_key(const SDL_KeyboardEvent& key, Command& out_cmd) const;
};

#endif