#ifndef COMMON_PACKET_HELPERS_H
#define COMMON_PACKET_HELPERS_H

#include <cstdint>
#include <string>
#include <vector>

uint8_t read_u8(const std::vector<uint8_t>& data, size_t& offset);
uint16_t read_u16(const std::vector<uint8_t>& data, size_t& offset);
uint32_t read_u32(const std::vector<uint8_t>& data, size_t& offset);
std::string read_string(const std::vector<uint8_t>& data, size_t& offset);
void validate_no_extra_bytes(const std::vector<uint8_t>& payload,
                             size_t offset);

void push_u8(std::vector<uint8_t>& payload, uint8_t value);
void push_u16(std::vector<uint8_t>& payload, uint16_t value);
void push_u32(std::vector<uint8_t>& payload, uint32_t value);
void push_string(std::vector<uint8_t>& payload, const std::string& value);

#endif  // COMMON_PACKET_HELPERS_H
