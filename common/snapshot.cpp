#include "snapshot.h"

#include "network/socket.h"

#include <arpa/inet.h>

Snapshot::Snapshot(
    uint16_t player_id,
    uint16_t x,
    uint16_t y):
    player_id(player_id),
    x(x),
    y(y),
    disconnect(false) {}

Snapshot::Snapshot():
    player_id(0),
    x(0),
    y(0),
    disconnect(true) {}

void Snapshot::send(
    Socket& socket) const {

    uint16_t net_player_id =
        htons(player_id);

    uint16_t net_x =
        htons(x);

    uint16_t net_y =
        htons(y);

    socket.sendall(
        &net_player_id,
        sizeof(net_player_id));

    socket.sendall(
        &net_x,
        sizeof(net_x));

    socket.sendall(
        &net_y,
        sizeof(net_y));
}

Snapshot Snapshot::recv(
    Socket& socket) {

    uint16_t net_player_id;
    uint16_t net_x;
    uint16_t net_y;

    socket.recvall(
        &net_player_id,
        sizeof(net_player_id));

    socket.recvall(
        &net_x,
        sizeof(net_x));

    socket.recvall(
        &net_y,
        sizeof(net_y));

    return Snapshot(
        ntohs(net_player_id),
        ntohs(net_x),
        ntohs(net_y));
}

bool Snapshot::is_disconnect() const {

    return disconnect;
}

uint16_t Snapshot::get_player_id() const {

    return player_id;
}

uint16_t Snapshot::get_x() const {

    return x;
}

uint16_t Snapshot::get_y() const {

    return y;
}