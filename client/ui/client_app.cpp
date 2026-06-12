#include "ui/client_app.h"

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL_image.h>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include <utility>

#include "audio/audio_manager.h"
#include "common/command/command.h"
#include "common/snapshot/snapshot.h"
#include "game/client_game_state.h"
#include "game/entity_keys.h"
#include "input/input_handler.h"
#include "net/server_connection.h"
#include "protocol/game_update.h"
#include "render/asset_paths.h"
#include "render/hud_renderer.h"
#include "render/world_renderer.h"
#include "ui/menu_screen.h"

using SDL2pp::Renderer;
using SDL2pp::SDL;
using SDL2pp::Window;

static constexpr Uint32 AWAIT_TIMEOUT_MS = 5000;
static constexpr Uint32 AWAIT_POLL_DELAY_MS = 10;
static constexpr int TARGET_FPS = 60;
static constexpr SDL_Color GAME_BG{34, 51, 34, 255};

ClientApp::ClientApp(ClientConfig config): config(std::move(config)) {}

int ClientApp::run() {
    try {
        SDL sdl(SDL_INIT_VIDEO);

        Uint32 flags = 0;
        if (config.fullscreen) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        Window window(config.window_title, SDL_WINDOWPOS_UNDEFINED,
                      SDL_WINDOWPOS_UNDEFINED, config.window_width,
                      config.window_height, flags);

        setup_window_icon(window);

        Renderer renderer(window, -1,
                          SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        MenuScreen menu(renderer, config);

        return menu_loop(menu, renderer);
    } catch (const std::exception& e) {
        std::cerr << "[Client] Error fatal: " << e.what() << std::endl;
        return 1;
    }
}

void ClientApp::setup_window_icon(Window& window) const {
    const auto icon_path = (std::filesystem::current_path() /
                            config.assets_path / assets::UI_ICON)
                               .lexically_normal();
    if (SDL_Surface* icon = IMG_Load(icon_path.string().c_str())) {
        SDL_SetWindowIcon(window.Get(), icon);
        SDL_FreeSurface(icon);
    }
}

int ClientApp::menu_loop(MenuScreen& menu, Renderer& renderer) {
    while (true) {
        if (menu.run_inicio() == MenuResult::QUIT) return 0;
        if (!login_loop(menu, renderer)) return 0;
    }
}

bool ClientApp::login_loop(MenuScreen& menu, Renderer& renderer) {
    std::string nick;
    while (true) {
        MenuResult rl = menu.run_login(nick);
        if (rl == MenuResult::QUIT) return false;
        if (rl == MenuResult::BACK) return true;

        ServerConnection connection(config.server_host, config.server_port);
        ClientGameState state(nick, config.map_width, config.map_height);

        ConnectResult cr = connect_and_login(menu, connection, state, nick);
        if (cr == ConnectResult::QUIT) {
            connection.send(Command::disconnect());
            connection.stop();
            return false;
        }
        if (cr == ConnectResult::BACK_TO_MENU) {
            connection.send(Command::disconnect());
            connection.stop();
            continue;
        }

        play_session(connection, renderer, state);
        connection.send(Command::disconnect());
        connection.stop();
        return false;
    }
}

ConnectResult ClientApp::connect_and_login(MenuScreen& menu,
                                           ServerConnection& connection,
                                           ClientGameState& state,
                                           const std::string& nick) {
    connection.send(Command::login(nick));
    int r = await_response(connection, state);

    if (r == 0) {
        std::string raza(keys::HUMANO);
        std::string clase(keys::MAGO);
        MenuResult rc = menu.run_create(raza, clase);
        if (rc == MenuResult::QUIT) return ConnectResult::QUIT;
        if (rc == MenuResult::BACK) return ConnectResult::BACK_TO_MENU;
        connection.send(Command::create_character(nick, raza, clase));
        r = await_response(connection, state);
    }

    if (r != 1) {
        std::cerr << "[Client] No se pudo iniciar sesion.\n";
        return ConnectResult::BACK_TO_MENU;
    }
    return ConnectResult::READY;
}

void ClientApp::play_session(ServerConnection& connection, Renderer& renderer,
                             ClientGameState& state) {
    AudioManager audio(config);
    WorldRenderer world_renderer(renderer, config);
    HudRenderer hud(renderer, config);
    InputHandler input;
    main_loop(connection, input, renderer, world_renderer, hud, state);
}

int ClientApp::await_response(ServerConnection& connection,
                              ClientGameState& state) {
    const uint32_t err_seq0 = state.get_error_seq();
    const Uint32 start = SDL_GetTicks();
    while (SDL_GetTicks() - start < AWAIT_TIMEOUT_MS) {
        GameUpdate update;
        bool got = false;
        while (connection.poll_update(update)) {
            got = true;
            if (update.disconnect) return -1;
            state.apply_update(update);
        }
        if (state.has_local_stats()) return 1;
        if (state.get_error_seq() != err_seq0) return 0;
        if (!got) SDL_Delay(AWAIT_POLL_DELAY_MS);
    }
    return -1;
}

void ClientApp::main_loop(ServerConnection& connection, InputHandler& input,
                          SDL2pp::Renderer& renderer, WorldRenderer& world,
                          HudRenderer& hud, ClientGameState& state) {
    const Uint32 frame_delay_ms = 1000 / TARGET_FPS;
    bool running = true;
    Uint32 last_ticks = SDL_GetTicks();

    while (running) {
        const Uint32 now = SDL_GetTicks();
        const Uint32 delta_ms = now - last_ticks;
        last_ticks = now;

        running = process_input(connection, input, state);
        if (running) {
            running = process_updates(connection, state);
        }

        renderer.SetDrawColor(GAME_BG.r, GAME_BG.g, GAME_BG.b, GAME_BG.a);
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
                              ClientGameState& state) {
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

            if (event.key.keysym.sym == SDLK_i) {
                state.toggle_inventory();
                continue;
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

bool ClientApp::process_updates(ServerConnection& connection,
                                ClientGameState& state) {
    state.begin_frame();
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
