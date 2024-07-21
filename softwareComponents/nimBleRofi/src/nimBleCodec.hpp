#pragma once

#include "pb_decode.h"
#include "pb_encode.h"
#include "protoc/rofi.pb.h"
#include <cstdint>
#include <rofi_hal.hpp>
#include <stddef.h>
#include <string>

namespace rofi::nimble {

class NimBleCodec {
private:
  rofi::hal::RoFI _localRoFI;

  void encodeDeviceCommand(DeviceCommand device);
  void encodeJointCommand(JointCommand joint);
  void encodeConnectorCommand(ConnectorCommand connector);

public:
  NimBleCodec(rofi::hal::RoFI localRoFI);

  size_t encodeRofiState(uint8_t *buffer, size_t buffer_size, uint32_t packetId,
                         int32_t rofiId, int32_t randomNum);
  int32_t decodeRofiRequest(const uint8_t *buffer, size_t message_length);

  size_t encodeRofiResponse(uint8_t *buffer, size_t buffer_size,
                            bool is_success, int32_t packet_id,
                            std::string message);
};
} // namespace rofi::nimble