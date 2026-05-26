#include "input/input_handler.h"

InputHandler::InputHandler() {
    load_default_bindings();
}

void InputHandler::load_default_bindings() {
    bindings[SDLK_UP] = KeyAction::MOVE_NORTH;
    bindings[SDLK_w] = KeyAction::MOVE_NORTH;

    bindings[SDLK_DOWN] = KeyAction::MOVE_SOUTH;
    bindings[SDLK_s] = KeyAction::MOVE_SOUTH;

    bindings[SDLK_RIGHT] = KeyAction::MOVE_EAST;
    bindings[SDLK_d] = KeyAction::MOVE_EAST;

    bindings[SDLK_LEFT] = KeyAction::MOVE_WEST;
    bindings[SDLK_a] = KeyAction::MOVE_WEST;
}

bool InputHandler::action_to_command(KeyAction action,
                                     ClientCommand& out_cmd) const {
    switch (action) {
        case KeyAction::MOVE_NORTH:
            out_cmd = ClientCommand::move(protocol::Direction::NORTH);
            return true;
        case KeyAction::MOVE_SOUTH:
            out_cmd = ClientCommand::move(protocol::Direction::SOUTH);
            return true;
        case KeyAction::MOVE_EAST:
            out_cmd = ClientCommand::move(protocol::Direction::EAST);
            return true;
        case KeyAction::MOVE_WEST:
            out_cmd = ClientCommand::move(protocol::Direction::WEST);
            return true;
    }
    return false;
}

bool InputHandler::process_key(const SDL_KeyboardEvent& key,
                               ClientCommand& out_cmd) const {
    if (key.type != SDL_KEYDOWN) {
        return false;
    }

    auto it = bindings.find(key.keysym.sym);
    if (it == bindings.end()) {
        return false;
    }

    return action_to_command(it->second, out_cmd);
}
