#include "input/input_handler.h"

InputHandler::InputHandler() {
    load_default_bindings();
    load_move_directions();
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

void InputHandler::load_move_directions() {
    move_directions[KeyAction::MOVE_NORTH] = protocol::Direction::NORTH;
    move_directions[KeyAction::MOVE_SOUTH] = protocol::Direction::SOUTH;
    move_directions[KeyAction::MOVE_EAST] = protocol::Direction::EAST;
    move_directions[KeyAction::MOVE_WEST] = protocol::Direction::WEST;
}

bool InputHandler::process_key(const SDL_KeyboardEvent& key,
                               Command& out_cmd) const {
    if (key.type != SDL_KEYDOWN) {
        return false;
    }
    // Prueba para cheats
    const bool ctrl = (key.keysym.mod & KMOD_CTRL) != 0;

    if (ctrl) {
        switch (key.keysym.sym) {
            case SDLK_g:
                out_cmd = Command::cheat_god();
                return true;

            case SDLK_m:
                out_cmd = Command::cheat_mana();
                return true;

            case SDLK_d:
                out_cmd = Command::cheat_die();
                return true;

            case SDLK_r:
                out_cmd = Command::cheat_resurrect();
                return true;

            default:
                return false;
        }
    }
    // Fin de pruebas
    auto it = bindings.find(key.keysym.sym);
    if (it == bindings.end()) {
        return false;
    }

    return action_to_command(it->second, out_cmd);
}

bool InputHandler::action_to_command(KeyAction action, Command& out_cmd) const {
    auto move_it = move_directions.find(action);
    if (move_it != move_directions.end()) {
        out_cmd = Command::move(static_cast<uint8_t>(move_it->second));
        return true;
    }
    return false;
}
