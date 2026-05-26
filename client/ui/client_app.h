#ifndef CLIENT_CLIENT_APP_H
#define CLIENT_CLIENT_APP_H

#include "config/client_config.h"

class ServerConnection;
class InputHandler;
class WorldRenderer;
class ClientGameState;

class ClientApp {
    private:
    ClientConfig config;

    void main_loop(ServerConnection& connection, InputHandler& input,
                   WorldRenderer& renderer, ClientGameState& state);
                   
    bool process_input(ServerConnection& connection, const InputHandler& input);

    bool process_updates(ServerConnection& connection, ClientGameState& state);

    public:
    explicit ClientApp(ClientConfig config);
    
    int run();
};

#endif
