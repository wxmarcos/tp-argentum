#ifndef CLIENT_SERVER_CONNECTION_H
#define CLIENT_SERVER_CONNECTION_H

#include <string>

#include "common/network/socket.h"
#include "common/queue.h"

#include "net/client_receiver.h"
#include "net/client_sender.h"
#include "protocol/client_command.h"
#include "protocol/game_update.h"

class ServerConnection {
    private:
    Socket socket;

    Queue<ClientCommand> commands_queue;
    Queue<GameUpdate> updates_queue;

    ClientSender sender;
    ClientReceiver receiver;

    bool running;

    public:
    ServerConnection(const std::string& hostname, const std::string& servname);

    ~ServerConnection();

    void send(const ClientCommand& cmd);

    bool poll_update(GameUpdate& out);

    void stop();

    ServerConnection(const ServerConnection&) = delete;
    ServerConnection& operator=(const ServerConnection&) = delete;
};

#endif