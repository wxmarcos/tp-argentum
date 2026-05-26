#include "command.h"
#include "network/socket.h"

Command Command::recv(Socket& socket) {

    uint16_t net_player_id;
    uint8_t type;
    uint16_t net_x;

    socket.recvall(&net_player_id, sizeof(net_player_id));
    socket.recvall(&type, sizeof(type));
    socket.recvall(&net_x, sizeof(net_x));

    return Command(
        ntohs(net_player_id),
        static_cast<CommandType>(type),
        ntohs(net_x)
    );
}

void Command::send(Socket& socket) const {

    uint16_t net_player_id = htons(player_id);
    uint8_t net_type = static_cast<uint8_t>(type);
    uint16_t net_x = htons(x);

    socket.sendall(&net_player_id, sizeof(net_player_id));
    socket.sendall(&net_type, sizeof(net_type));
    socket.sendall(&net_x, sizeof(net_x));
}