#ifndef CLIENT_WIRE_READER_H
#define CLIENT_WIRE_READER_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace wire {

uint8_t read_u8(const std::vector<uint8_t>& data, size_t& offset);

uint16_t read_u16(const std::vector<uint8_t>& data, size_t& offset);

uint32_t read_u32(const std::vector<uint8_t>& data, size_t& offset);

std::string read_string(const std::vector<uint8_t>& data, size_t& offset);

}

#endif