#include "common/command/packet_helpers.h"

#include <arpa/inet.h>

#include <cstring>
#include <stdexcept>

uint8_t read_u8(const std::vector<uint8_t>& data, size_t& offset) {
    if (offset + 1 > data.size()) {
        throw std::runtime_error("payload incompleto leyendo uint8");
    }

    return data[offset++];
}

uint16_t read_u16(const std::vector<uint8_t>& data, size_t& offset) {
    if (offset + 2 > data.size()) {
        throw std::runtime_error("payload incompleto leyendo uint16");
    }

    uint16_t net_value;
    std::memcpy(&net_value, data.data() + offset, sizeof(net_value));
    offset += 2;

    return ntohs(net_value);
}

uint32_t read_u32(const std::vector<uint8_t>& data, size_t& offset) {
    if (offset + 4 > data.size()) {
        throw std::runtime_error("payload incompleto leyendo uint32");
    }

    uint32_t net_value;
    std::memcpy(&net_value, data.data() + offset, sizeof(net_value));
    offset += 4;

    return ntohl(net_value);
}

std::string read_string(const std::vector<uint8_t>& data, size_t& offset) {
    uint16_t len = read_u16(data, offset);

    if (offset + len > data.size()) {
        throw std::runtime_error("payload incompleto leyendo string");
    }

    std::string value(reinterpret_cast<const char*>(data.data() + offset), len);

    offset += len;
    return value;
}

void validate_no_extra_bytes(const std::vector<uint8_t>& payload,
                             size_t offset) {
    if (offset != payload.size()) {
        throw std::runtime_error("payload con bytes extra");
    }
}

void push_u8(std::vector<uint8_t>& payload, uint8_t value) {
    payload.push_back(value);
}

void push_u16(std::vector<uint8_t>& payload, uint16_t value) {
    uint16_t net_value = htons(value);
    auto* bytes = reinterpret_cast<uint8_t*>(&net_value);
    payload.push_back(bytes[0]);
    payload.push_back(bytes[1]);
}

void push_u32(std::vector<uint8_t>& payload, uint32_t value) {
    uint32_t net_value = htonl(value);
    auto* bytes = reinterpret_cast<uint8_t*>(&net_value);
    payload.push_back(bytes[0]);
    payload.push_back(bytes[1]);
    payload.push_back(bytes[2]);
    payload.push_back(bytes[3]);
}

void push_string(std::vector<uint8_t>& payload, const std::string& value) {
    push_u16(payload, static_cast<uint16_t>(value.size()));
    payload.insert(payload.end(), value.begin(), value.end());
}
