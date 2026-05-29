#include "ui/client_app.h"

#include <exception>
#include <iostream>
#include <utility>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#include "audio/audio_manager.h"
#include "common/command.h"
#include "game/client_game_state.h"
#include "input/input_handler.h"
#include "net/server_connection.h"
#include "protocol/game_update.h"
#include "render/world_renderer.h"

using namespace SDL2pp;

ClientApp::ClientApp(ClientConfig config): config(std::move(config)) {}

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
        ClientGameState state(config.start_x, config.start_y, config.map_width,
                              config.map_height);

        ServerConnection connection(config.server_host, config.server_port);
        std::cout << "[Client] Conectado a " << config.server_host << ":"
                  << config.server_port << "\n";

        main_loop(connection, input, world_renderer, state);

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

    while (running) {
        Uint32 frame_start = SDL_GetTicks();

        running = process_input(connection, input);
        if (running) {
            running = process_updates(connection, state);
        }

        renderer.render(state);

        Uint32 elapsed = SDL_GetTicks() - frame_start;
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
            return false;
        }
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                return false;
            }
            Command cmd(0, CommandType::Disconnect);
            if (input.process_key(event.key, cmd)) {
                connection.send(cmd);
            }
        }
    }
    return true;
}

bool ClientApp::process_updates(ServerConnection& connection,
                                ClientGameState& state) {
    GameUpdate update;
    while (connection.poll_update(update)) {
        if (update.disconnect) {
            std::cout << "[Client] El servidor cerro la conexion.\n";
            return false;
        }
        state.apply_update(update);
    }
    return true;
}