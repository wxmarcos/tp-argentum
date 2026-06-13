#include "ui/client_app.h"

#include <algorithm>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL_image.h>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include <utility>

#include "audio/audio_manager.h"
#include "audio/audio_assets.h"
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
#include "ui/console.h"

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

        AudioManager audio(config);
        load_audio(audio);
        audio.play_music(audio_assets::MUSIC);

        return menu_loop(menu, renderer, audio);
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

int ClientApp::menu_loop(MenuScreen& menu, Renderer& renderer,
                         AudioManager& audio) {
    while (true) {
        if (menu.run_inicio() == MenuResult::QUIT) return 0;
        if (!login_loop(menu, renderer, audio)) return 0;
    }
}

bool ClientApp::login_loop(MenuScreen& menu, Renderer& renderer,
                           AudioManager& audio) {
    std::string nick;
    while (true) {
        MenuResult rl = menu.run_login(nick);
        if (rl == MenuResult::QUIT) return false;
        if (rl == MenuResult::BACK) return true;

        try {
            ServerConnection connection(config.server_host,
                                        config.server_port);
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

            play_session(connection, renderer, state, audio);
            connection.send(Command::disconnect());
            connection.stop();
            return false;
        } catch (const std::exception& e) {
            std::cerr << "[Client] No se pudo conectar al servidor: "
                      << e.what() << "\n";
            continue;
        }
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
                             ClientGameState& state, AudioManager& audio) {
    was_meditating = false;
    prev_x = -1;
    prev_y = -1;
    prev_level = -1;

    WorldRenderer world_renderer(renderer, config);
    HudRenderer hud(renderer, config);
    InputHandler input;
    Console console;
    main_loop(connection, input, renderer, world_renderer, hud, state, console,
              audio);
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
                          HudRenderer& hud, ClientGameState& state,
                          Console& console, AudioManager& audio) {
    const Uint32 frame_delay_ms = 1000 / TARGET_FPS;
    bool running = true;
    Uint32 last_ticks = SDL_GetTicks();

    while (running) {
        const Uint32 now = SDL_GetTicks();
        const Uint32 delta_ms = now - last_ticks;
        last_ticks = now;

        running = process_input(connection, input, state, console);
        if (running) {
            running = process_updates(connection, state);
        }
        if (running) {
            update_audio(audio, state);
        }

        renderer.SetDrawColor(GAME_BG.r, GAME_BG.g, GAME_BG.b, GAME_BG.a);
        renderer.Clear();
        world.render(state, delta_ms);
        hud.render(state, console);
        renderer.Present();

        const Uint32 elapsed = SDL_GetTicks() - now;
        if (elapsed < frame_delay_ms) {
            SDL_Delay(frame_delay_ms - elapsed);
        }
    }
}

bool ClientApp::process_input(ServerConnection& connection,
                              const InputHandler& input,
                              ClientGameState& state, Console& console) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            connection.send(Command::disconnect());
            return false;
        }

        if (console.is_open()) {
            handle_console_event(event, console, connection);
            continue;
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
            if (event.key.keysym.sym == SDLK_RETURN ||
                event.key.keysym.sym == SDLK_KP_ENTER) {
                console.open();
                SDL_StartTextInput();
                continue;
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

void ClientApp::handle_console_event(const SDL_Event& event, Console& console,
                                     ServerConnection& connection) {
    if (event.type == SDL_TEXTINPUT) {
        console.append(event.text.text);
        return;
    }
    if (event.type != SDL_KEYDOWN) {
        return;
    }
    const SDL_Keycode key = event.key.keysym.sym;
    if (key == SDLK_ESCAPE) {
        console.close();
        SDL_StopTextInput();
    } else if (key == SDLK_BACKSPACE) {
        console.backspace();
    } else if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
        submit_console(console, connection);
        SDL_StopTextInput();
    }
}

void ClientApp::submit_console(Console& console, ServerConnection& connection) {
    const std::string line = console.take();
    console.close();
    if (auto cmd = parser.parse(line)) {
        connection.send(*cmd);
    }
}

void ClientApp::handle_click(ServerConnection& connection,
                             const ClientGameState& state, int mouse_x,
                             int mouse_y) {
    if (!state.has_local_position()) {
        return;
    }

    const int ts = config.tile_size;

    const int cam_offset_x =
        config.game_area_width() / 2 - state.get_local_x() * ts - ts / 2;
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

void ClientApp::load_audio(AudioManager& audio) {
    audio.load_effect(audio_assets::KEY_ATTACK, audio_assets::PATH_ATTACK);
    audio.load_effect(audio_assets::KEY_DEATH, audio_assets::PATH_DEATH);
    audio.load_effect(audio_assets::KEY_MEDITATE, audio_assets::PATH_MEDITATE);
    audio.load_effect(audio_assets::KEY_HIT, audio_assets::PATH_HIT);
    audio.load_effect(audio_assets::KEY_STEP, audio_assets::PATH_STEP);
    audio.load_effect(audio_assets::KEY_LEVELUP, audio_assets::PATH_LEVELUP);
}

void ClientApp::play_event_sounds(AudioManager& audio,
                                  const ClientGameState& state) {
    if (!state.has_local_position()) {
        return;
    }
    const int lx = state.get_local_x();
    const int ly = state.get_local_y();
    int played = 0;
    for (const EffectSpawn& spawn : state.get_effect_spawns()) {
        if (played >= audio_assets::MAX_SOUNDS_PER_FRAME) {
            break;
        }
        const bool on_me = (static_cast<int>(spawn.x) == lx &&
                            static_cast<int>(spawn.y) == ly);
        const char* key = nullptr;
        switch (spawn.kind) {
            case EffectKind::AtaqueComunRojo:
            case EffectKind::AtaqueComunGris:
            case EffectKind::AtaqueComunDorado:
            case EffectKind::AtaqueBaculoComun:
            case EffectKind::AtaqueBaculoDorado:
                key = on_me ? audio_assets::KEY_HIT : audio_assets::KEY_ATTACK;
                break;
            case EffectKind::EfectoMorir:
                key = audio_assets::KEY_DEATH;
                break;
            default:
                break;
        }
        if (key == nullptr) {
            continue;
        }
        const int dist = std::max(std::abs(lx - static_cast<int>(spawn.x)),
                                  std::abs(ly - static_cast<int>(spawn.y)));
        audio.play_effect_at(key, dist);
        ++played;
    }
}

void ClientApp::update_audio(AudioManager& audio,
                             const ClientGameState& state) {
    play_event_sounds(audio, state);

    const bool meditating_now =
        state.has_local_position() &&
        state.is_meditating(state.get_local_nick());
    if (meditating_now && !was_meditating) {
        audio.play_effect(audio_assets::KEY_MEDITATE);
    }
    was_meditating = meditating_now;

    if (state.has_local_position()) {
        const int x = state.get_local_x();
        const int y = state.get_local_y();
        if (prev_x >= 0 && (x != prev_x || y != prev_y)) {
            audio.play_effect(audio_assets::KEY_STEP, audio_assets::STEP_VOLUME);
        }
        prev_x = x;
        prev_y = y;
    }

    if (state.has_local_stats()) {
        const int level = state.get_local_stats().nivel;
        if (prev_level >= 0 && level > prev_level) {
            audio.play_effect(audio_assets::KEY_LEVELUP);
        }
        prev_level = level;
    }
}
