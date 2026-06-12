#ifndef CLIENT_CLIENT_APP_H
#define CLIENT_CLIENT_APP_H

#include <string>

#include "config/client_config.h"
#include "ui/connect_result.h"

namespace SDL2pp {
class Renderer;
class Window;
}

class ServerConnection;
class InputHandler;
class WorldRenderer;
class HudRenderer;
class ClientGameState;
class Snapshot;
class MenuScreen;

class ClientApp {
    private:
    ClientConfig config;

    void setup_window_icon(SDL2pp::Window& window) const;
    int menu_loop(MenuScreen& menu, SDL2pp::Renderer& renderer);
    bool login_loop(MenuScreen& menu, SDL2pp::Renderer& renderer);
    ConnectResult connect_and_login(MenuScreen& menu,
                                    ServerConnection& connection,
                                    ClientGameState& state,
                                    const std::string& nick);
    void play_session(ServerConnection& connection, SDL2pp::Renderer& renderer,
                      ClientGameState& state);

    void main_loop(ServerConnection& connection, InputHandler& input,
                   SDL2pp::Renderer& renderer, WorldRenderer& world,
                   HudRenderer& hud, ClientGameState& state);

    bool process_input(ServerConnection& connection, const InputHandler& input,
                       ClientGameState& state);

    void handle_click(ServerConnection& connection,
                      const ClientGameState& state, int mouse_x, int mouse_y);

    bool process_updates(ServerConnection& connection, ClientGameState& state);

    int await_response(ServerConnection& connection, ClientGameState& state);

    public:
    explicit ClientApp(ClientConfig config);

    int run();
};

#endif