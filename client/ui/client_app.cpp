#include "ui/client_app.h"

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL_image.h>
#include <filesystem>
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
#include "render/hud_renderer.h"
#include "ui/menu_screen.h"

using SDL2pp::Renderer;
using SDL2pp::SDL;
using SDL2pp::Window;

ClientApp::ClientApp(ClientConfig config):
    config(std::move(config)), awaiting_login(true), tried_login(false) {}

int ClientApp::run() {
    try {
        SDL sdl(SDL_INIT_VIDEO);

        Uint32 flags = 0;
        if (config.fullscreen) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        Window window(config.window_title, SDL_WINDOWPOS_UNDEFINED,
                      SDL_WINDOWPOS_UNDEFINED, config.window_width,
                      config.window_height, flags);

        const auto icon_path =
            (std::filesystem::current_path() / config.assets_path / "ui/icon.png")
                .lexically_normal();
        if (SDL_Surface* icon = IMG_Load(icon_path.string().c_str())) {
            SDL_SetWindowIcon(window.Get(), icon);
            SDL_FreeSurface(icon);
        }
        
        Renderer renderer(window, -1,
                          SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        MenuScreen menu(renderer, config);

        while (true) {
            if (!menu.run_inicio()) return 0;

            std::string nick;
            bool volver_a_inicio = false;
            while (!volver_a_inicio) {
                if (!menu.run_login(nick)) {
                    volver_a_inicio = true;
                    break;
                }

                ServerConnection connection(config.server_host,
                                            config.server_port);
                ClientGameState state(nick, config.map_width,
                                      config.map_height);

                connection.send(Command::login(nick));
                int r = await_response(connection, state);

                if (r == 0) {
                    std::string raza = "humano";
                    std::string clase = "mago";
                    if (!menu.run_create(raza, clase)) {
                        connection.send(Command::disconnect());
                        connection.stop();
                        continue;
                    }
                    connection.send(
                        Command::create_character(nick, raza, clase));
                    r = await_response(connection, state);
                }

                if (r != 1) {
                    std::cerr << "[Client] No se pudo iniciar sesion.\n";
                    connection.send(Command::disconnect());
                    connection.stop();
                    continue;
                }

                AudioManager audio(config);
                WorldRenderer world_renderer(renderer, config);
                world_renderer.load_map(
                    config.map_name_for(state.get_current_map_id()));
                HudRenderer hud(renderer, config);
                InputHandler input;

                main_loop(connection, input, renderer, world_renderer, hud,
                          state);

                connection.send(Command::disconnect());
                connection.stop();
                return 0;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[Client] Error fatal: " << e.what() << std::endl;
        return 1;
    }
}

int ClientApp::await_response(ServerConnection& connection,
                              ClientGameState& state) {
    const uint32_t err_seq0 = state.get_error_seq();
    const Uint32 start = SDL_GetTicks();
    while (SDL_GetTicks() - start < 5000) {
        GameUpdate update;
        bool got = false;
        while (connection.poll_update(update)) {
            got = true;
            if (update.disconnect) return -1;
            state.apply_update(update);
        }
        if (state.has_local_stats()) return 1;
        if (state.get_error_seq() != err_seq0) return 0;
        if (!got) SDL_Delay(10);
    }
    return -1;
}

void ClientApp::main_loop(ServerConnection& connection, InputHandler& input,
                          SDL2pp::Renderer& renderer, WorldRenderer& world,
                          HudRenderer& hud, ClientGameState& state) {
    const Uint32 frame_delay_ms = 1000 / 60;
    bool running = true;
    Uint32 last_ticks = SDL_GetTicks();
    uint16_t last_map_id = state.get_current_map_id();

    while (running) {
        const Uint32 now = SDL_GetTicks();
        const Uint32 delta_ms = now - last_ticks;
        last_ticks = now;

        running = process_input(connection, input, state);
        if (running) {
            running = process_updates(connection, state);
        }

        if (state.get_current_map_id() != last_map_id) {
            last_map_id = state.get_current_map_id();
            world.load_map(config.map_name_for(last_map_id));
        }

        renderer.SetDrawColor(34, 51, 34, 255);
        renderer.Clear();
        world.render(state, delta_ms);
        hud.render(state);
        renderer.Present();

        const Uint32 elapsed = SDL_GetTicks() - now;
        if (elapsed < frame_delay_ms) {
            SDL_Delay(frame_delay_ms - elapsed);
        }
    }
}

bool ClientApp::process_input(ServerConnection& connection,
                              const InputHandler& input,
                              const ClientGameState& state) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            connection.send(Command::disconnect());
            return false;
        }

        if (event.type == SDL_MOUSEBUTTONDOWN &&
            event.button.button == SDL_BUTTON_LEFT) {
            handle_click(connection, state, event.button.x, event.button.y);
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

void ClientApp::handle_click(ServerConnection& connection,
                             const ClientGameState& state, int mouse_x,
                             int mouse_y) {
    if (!state.has_local_position()) {
        return;
    }
    const int ts = config.tile_size;
    const int cam_offset_x =
        config.window_width / 2 - state.get_local_x() * ts - ts / 2;
    const int cam_offset_y =
        config.window_height / 2 - state.get_local_y() * ts - ts / 2;

    const int tile_x = (mouse_x - cam_offset_x) / ts;
    const int tile_y = (mouse_y - cam_offset_y) / ts;
    if (tile_x < 0 || tile_y < 0) {
        return;
    }

    std::string target;
    if (state.entity_at(static_cast<uint16_t>(tile_x),
                        static_cast<uint16_t>(tile_y), target)) {
        connection.send(Command::attack(target));
    }
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
