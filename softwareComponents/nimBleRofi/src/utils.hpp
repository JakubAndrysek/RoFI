#pragma once

#include <cstdint>
#include <iomanip>
#include <string.h>

std::string buffer_to_hex(const uint8_t *buffer, size_t length);
std::string getMACAddress();