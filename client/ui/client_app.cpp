#include "ui/client_app.h"

#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>
#include <exception>
#include <iostream>
#include <utility>

#include "audio/audio_manager.h"
#include "common/command/command.h"
#include "common/snapshot/snapshot.h"
#include "game/client_game_state.h"
#include "input/input_handler.h"
#include "net/server_connection.h"
#include "protocol/game_update.h"
#include "render/world_renderer.h"

using SDL2pp::Renderer;
using SDL2pp::SDL;
using SDL2pp::Window;

ClientApp::ClientApp(ClientConfig config):
    config(std::move(config)), awaiting_login(true), tried_login(false) {}

int ClientApp::run() {
    try {
        SDL sdl(SDL_INIT_VIDEO);

        Uint32 window_flags = SDL_WINDOW_RESIZABLE;
        if (config.fullscreen) {
            window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        }

        Window window(config.window_title, SDL_WINDOWPOS_UNDEFINED,
                      SDL_WINDOWPOS_UNDEFINED, config.window_width,
                      config.window_height, window_flags);

        Renderer renderer(window, -1,
                          SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        AudioManager audio(config);
        WorldRenderer world_renderer(renderer, config);
        InputHandler input;
        ClientGameState state(config.character_nick, config.map_width,
                              config.map_height);

        ServerConnection connection(config.server_host, config.server_port);
        std::cout << "[Client] Conectado a " << config.server_host << ":"
                  << config.server_port << "\n";

        connection.send(Command::create_character(config.character_nick,
                                                  config.character_raza,
                                                  config.character_clase));
        main_loop(connection, input, world_renderer, state);

        connection.send(Command::disconnect());
        connection.stop();
        std::cout << "[Client] Cerrado limpiamente.\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "[Client] Error fatal: " << e.what() << std::endl;
        return 1;
    }
}

void ClientApp::main_loop(ServerConnection& connection, InputHandler& input,
                          WorldRenderer& renderer, ClientGameState& state) {
    const Uint32 frame_delay_ms = 1000 / 60;
    bool running = true;
    Uint32 last_ticks = SDL_GetTicks();

    while (running) {
        const Uint32 now = SDL_GetTicks();
        const Uint32 delta_ms = now - last_ticks;
        last_ticks = now;

        running = process_input(connection, input);
        if (running) {
            running = process_updates(connection, state);
        }

        renderer.render(state, delta_ms);

        const Uint32 elapsed = SDL_GetTicks() - now;
        if (elapsed < frame_delay_ms) {
            SDL_Delay(frame_delay_ms - elapsed);
        }
    }
}

bool ClientApp::process_input(ServerConnection& connection,
                              const InputHandler& input) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            connection.send(Command::disconnect());
            return false;
        }

        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE ||
                event.key.keysym.sym == SDLK_q) {
                connection.send(Command::disconnect());
                return false;
            }

            Command cmd = Command::disconnect();

            if (input.process_key(event.key, cmd)) {
                connection.send(cmd);
            }
        }
    }

    return true;
}

void ClientApp::process_login_response(ServerConnection& connection,
                                       const Snapshot& snapshot) {
    if (!awaiting_login) {
        return;
    }

    if (snapshot.is_player_stats() &&
        snapshot.get_nick() == config.character_nick) {
        awaiting_login = false;
        return;
    }

    if (snapshot.is_error_message() && !tried_login) {
        tried_login = true;
        std::cout << "[Client] CREATE_CHARACTER rechazado, reintentando "
                     "con LOGIN.\n";
        connection.send(Command::login(config.character_nick));
    }
}

bool ClientApp::process_updates(ServerConnection& connection,
                                ClientGameState& state) {
    state.begin_frame();
    GameUpdate update;
    while (connection.poll_update(update)) {
        if (update.disconnect) {
            std::cout << "[Client] El servidor cerro la conexion.\n";
            return false;
        }
        if (update.snapshot.has_value()) {
            process_login_response(connection, *update.snapshot);
        }
        state.apply_update(update);
    }
    return true;
}
