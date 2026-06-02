#include "common/network/socket_helpers.h"

#include <arpa/inet.h>
#include <stdexcept>

void send_u8(Socket& socket, uint8_t value) {
    socket.sendall(&value, sizeof(value));
}

void send_u16(Socket& socket, uint16_t value) {
    uint16_t net_value = htons(value);
    socket.sendall(&net_value, sizeof(net_value));
}

void send_u32(Socket& socket, uint32_t value) {
    uint32_t net_value = htonl(value);
    socket.sendall(&net_value, sizeof(net_value));
}

uint8_t recv_u8(Socket& socket) {
    uint8_t value;
    int received = socket.recvall(&value, sizeof(value));
    if (received == 0)
        throw std::runtime_error("socket cerrado leyendo uint8");
    return value;
}

uint16_t recv_u16(Socket& socket) {
    uint16_t net_value;
    int received = socket.recvall(&net_value, sizeof(net_value));
    if (received == 0)
        throw std::runtime_error("socket cerrado leyendo uint16");
    return ntohs(net_value);
}

uint32_t recv_u32(Socket& socket) {
    uint32_t net_value;
    int received = socket.recvall(&net_value, sizeof(net_value));
    if (received == 0)
        throw std::runtime_error("socket cerrado leyendo uint32");
    return ntohl(net_value);
}

void send_string(Socket& socket, const std::string& value) {
    send_u16(socket, static_cast<uint16_t>(value.size()));
    if (!value.empty()) {
        socket.sendall(value.data(), value.size());
    }
}

std::string recv_string(Socket& socket) {
    uint16_t len = recv_u16(socket);

    std::string value;
    value.resize(len);

    if (len > 0) {
        int received = socket.recvall(value.data(), len);
        if (received == 0)
            throw std::runtime_error("socket cerrado leyendo string");
    }

    return value;
}
