#ifndef CLIENT_CLIENT_APP_H
#define CLIENT_CLIENT_APP_H

#include "config/client_config.h"

class ServerConnection;
class InputHandler;
class WorldRenderer;
class ClientGameState;
class Snapshot;

class ClientApp {
private:
    ClientConfig config;
    bool awaiting_login;
    bool tried_login;

    void main_loop(ServerConnection& connection, InputHandler& input,
                   WorldRenderer& renderer, ClientGameState& state);

    bool process_input(ServerConnection& connection, const InputHandler& input);

    bool process_updates(ServerConnection& connection, ClientGameState& state);

    void process_login_response(ServerConnection& connection,
                                const Snapshot& snapshot);

public:
    explicit ClientApp(ClientConfig config);

    int run();
};

#endif