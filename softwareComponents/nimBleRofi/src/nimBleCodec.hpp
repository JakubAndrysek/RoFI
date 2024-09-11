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

  void encodeMessage(Message message);
  void encodeDeviceCommand(DeviceCommand device);
  void encodeJointCommand(JointCommand joint);
  void encodeConnectorCommand(ConnectorCommand connector);

public:
  NimBleCodec(rofi::hal::RoFI localRoFI);

  size_t encodeRofiState(uint8_t *buffer, size_t buffer_size,
                         uint32_t packetId);

  size_t encodeRofiStateError(uint8_t *buffer, size_t buffer_size,
                              uint32_t packet_id, std::string error_message);

  int32_t decodeRofiRequest(const uint8_t *buffer, size_t message_length);

  size_t encodeRofiResponse(uint8_t *buffer, size_t buffer_size,
                            int32_t packet_id, bool is_success,
                            std::string message);
};
} // namespace rofi::nimble