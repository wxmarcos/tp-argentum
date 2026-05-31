#include "protocol/wire_reader.h"

#include <cstring>
#include <stdexcept>

#include <arpa/inet.h>

namespace wire {

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
    std::string value(reinterpret_cast<const char*>(data.data() + offset),
                      len);
    offset += len;
    return value;
}

}
