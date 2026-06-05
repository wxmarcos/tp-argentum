#ifndef COMMON_NETWORK_SOCKET_HELPERS_H
#define COMMON_NETWORK_SOCKET_HELPERS_H

#include <cstdint>
#include <string>

#include "common/network/socket.h"

void send_u8(Socket& socket, uint8_t value);
void send_u16(Socket& socket, uint16_t value);
void send_u32(Socket& socket, uint32_t value);

uint8_t recv_u8(Socket& socket);
uint16_t recv_u16(Socket& socket);
uint32_t recv_u32(Socket& socket);

void send_string(Socket& socket, const std::string& value);
std::string recv_string(Socket& socket);

#endif  // COMMON_NETWORK_SOCKET_HELPERS_H
