#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <cstdint>

class Socket;

class Snapshot {

private:

    uint16_t player_id;

    uint16_t x;
    uint16_t y;

    bool disconnect;

public:

    Snapshot(
        uint16_t player_id,
        uint16_t x,
        uint16_t y);

    Snapshot();

    void send(
        Socket& socket) const;

    static Snapshot recv(
        Socket& socket);

    bool is_disconnect() const;

    uint16_t get_player_id() const;

    uint16_t get_x() const;

    uint16_t get_y() const;
};

#endif