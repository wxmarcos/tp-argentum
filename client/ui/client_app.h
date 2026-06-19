#ifndef CLIENT_CLIENT_APP_H
#define CLIENT_CLIENT_APP_H

#include <SDL2/SDL.h>
#include <string>

#include "config/client_config.h"
#include "ui/command_parser.h"
#include "ui/connect_result.h"

namespace SDL2pp {
class Renderer;
class Window;
}

class AudioManager;
class ServerConnection;
class InputHandler;
class WorldRenderer;
class HudRenderer;
class ClientGameState;
class Snapshot;
class MenuScreen;
class Console;

class ClientApp {
    private:
    ClientConfig config;
    CommandParser parser;

    bool was_meditating = false;
    int prev_x = -1;
    int prev_y = -1;
    int prev_level = -1;

    void setup_window_icon(SDL2pp::Window& window) const;

    int menu_loop(MenuScreen& menu, SDL2pp::Renderer& renderer,
                  AudioManager& audio);

    bool login_loop(MenuScreen& menu, SDL2pp::Renderer& renderer,
                    AudioManager& audio);

    ConnectResult connect_and_login(MenuScreen& menu,
                                    ServerConnection& connection,
                                    ClientGameState& state,
                                    const std::string& nick);

    void play_session(ServerConnection& connection, SDL2pp::Renderer& renderer,
                      ClientGameState& state, AudioManager& audio);

    void main_loop(ServerConnection& connection, InputHandler& input,
                   SDL2pp::Renderer& renderer, WorldRenderer& world,
                   HudRenderer& hud, ClientGameState& state, Console& console,
                   AudioManager& audio);

    bool process_input(ServerConnection& connection, const InputHandler& input,
                       WorldRenderer& world, HudRenderer& hud,
                       ClientGameState& state, Console& console,
                       AudioManager& audio);

    void handle_console_event(const SDL_Event& event, Console& console,
                              ServerConnection& connection, AudioManager& audio);
                              
    void submit_console(Console& console, ServerConnection& connection,
                        AudioManager& audio);

    void handle_click(ServerConnection& connection, WorldRenderer& world,
                      HudRenderer& hud, AudioManager& audio,
                      const ClientGameState& state, int mouse_x, int mouse_y);

    bool process_updates(ServerConnection& connection, ClientGameState& state);

    int await_response(ServerConnection& connection, ClientGameState& state);
    
    void load_audio(AudioManager& audio);

    void play_event_sounds(AudioManager& audio, const ClientGameState& state);

    void update_audio(AudioManager& audio, const ClientGameState& state);

    public:
    explicit ClientApp(ClientConfig config);

    int run();
};

#endif