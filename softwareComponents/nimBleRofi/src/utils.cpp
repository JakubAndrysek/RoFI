#include "utils.hpp"
#include <iomanip>
#include <sstream>
#include <iostream>
#include <cstring>
#include <sstream>
#include <iomanip>
#include "esp_system.h"
#include "esp_mac.h"

std::string buffer_to_hex(const uint8_t *buffer, size_t length) {
  std::stringstream ss;
  ss << std::hex << std::setfill('0');
  for (size_t i = 0; i < length; ++i) {
    ss << std::setw(2) << static_cast<int>(buffer[i]);
  }
  return ss.str();
}

std::string getMACAddress() {
    uint8_t mac[6];
    // Get the MAC address (ESP_MAC_WIFI_STA is the default Wi-Fi MAC)
    esp_read_mac(mac, ESP_MAC_WIFI_STA);

    // Convert the MAC address to a string
    std::stringstream macStr;
    for (int i = 0; i < 6; ++i) {
        if (i != 0) {
            macStr << ":";
        }
        macStr << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(mac[i]);
    }

    return macStr.str();
}