#include "nimBleCodec.hpp"

#include "esp_log.h"
#include "esp_system.h"
#include <stdexcept>

#define TAG "NIMBLE_CODEC"

namespace rofi::nimble {

NimBleCodec::NimBleCodec(rofi::hal::RoFI localRoFI) : _localRoFI(localRoFI) {}

/**
 * Callback function to encode a string
 * \param stream The stream to write to
 * \param field The field to encode
 */
bool encode_string(pb_ostream_t *stream, const pb_field_t *field,
                   void *const *arg) {
  const char *str = (const char *)(*arg);
  if (!pb_encode_tag_for_field(stream, field)) {
    return false;
  }
  return pb_encode_string(stream, (const uint8_t *)str, strlen(str));
}

size_t NimBleCodec::encodeRofiState(uint8_t *buffer, size_t buffer_size,
                                    uint32_t packet_id, int32_t rofi_id,
                                    int32_t random_number) {
  RofiState rofi_state = RofiState_init_zero;
  rofi_state.packet_id = packet_id;
  rofi_state.rofi_id = rofi_id;
  rofi_state.random_number = random_number;

  pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_size);
  if (!pb_encode(&stream, RofiState_fields, &rofi_state)) {
    ESP_LOGE(TAG, "Encoding failed: %s", PB_GET_ERROR(&stream));
    throw std::runtime_error("Encoding failed");
  }

  return stream.bytes_written;
}

void NimBleCodec::encodeDeviceCommand(DeviceCommand device) {
  switch (device.command) {
  case DeviceCommandType_REBOOT:
    ESP_LOGI(TAG, "Device Command: REBOOT");
    esp_restart();
    break;

  case DeviceCommandType_SET_ID:
    ESP_LOGI(TAG, "Device Command: SET_ID=%d", device.command_type.set_id);
    break;

  case DeviceCommandType_SET_RANDOM_NUMBER:
    ESP_LOGI(TAG, "Device Command: SET_RANDOM_NUMBER=%d",
             device.command_type.set_random_number);
    break;
  }
}

void NimBleCodec::encodeJointCommand(JointCommand joint) {
  switch (joint.command) {
  case JointCommandType_SET_JOINT_VELOCITY:
    ESP_LOGI(TAG, "Joint (ID=%d) Command: SET_JOINT_VELOCITY=%d",
             joint.joint_id, joint.command_type.set_velocity);
    _localRoFI.getJoint(joint.joint_id)
        .setVelocity(joint.command_type.set_velocity);
    break;

  case JointCommandType_SET_JOINT_POSITION:
    ESP_LOGI(TAG, "Joint (ID=%d) Command: SET_JOINT_POSITION=%d",
             joint.joint_id, joint.command_type.set_position);
    _localRoFI.getJoint(joint.joint_id)
        .setPosition(joint.command_type.set_position.position,
                     joint.command_type.set_position.velocity,
                     [](rofi::hal::Joint) {});
    break;

  case JointCommandType_SET_JOINT_TORQUE:
    ESP_LOGI(TAG, "Joint (ID=%d) Command: SET_JOINT_TORQUE=%d", joint.joint_id,
             joint.command_type.set_torque);
    _localRoFI.getJoint(joint.joint_id)
        .setTorque(joint.command_type.set_torque);
    break;
  }
}

void NimBleCodec::encodeConnectorCommand(ConnectorCommand connector) {
  switch (connector.command) {
  case ConnectorCommandType_SET_POSITION:
    ESP_LOGI(TAG, "Connector (ID=%d) Command: SET_POSITION=%d",
             connector.connector_id, connector.command_type.set_position);
    break;

  case ConnectorCommandType_CONNECT_POWER:
    ESP_LOGI(TAG, "Connector (ID=%d) Command: CONNECT_POWER=%d",
             connector.connector_id, connector.command_type.connect_power);
    break;

  case ConnectorCommandType_DISCONNECT_POWER:
    ESP_LOGI(TAG, "Connector (ID=%d) Command: DISCONNECT_POWER=%d",
             connector.connector_id, connector.command_type.disconnect_power);
    break;

  case ConnectorCommandType_SET_DISTANCE_MODE:
    ESP_LOGI(TAG, "Connector (ID=%d) Command: SET_DISTANCE_MODE=%d",
             connector.connector_id, connector.command_type.set_distance_mode);
    break;

  default:
    ESP_LOGE(TAG, "Unknown command type");
  }
}

int32_t NimBleCodec::decodeRofiRequest(const uint8_t *buffer,
                                       size_t message_length) {
  RofiRequest rofi_request = RofiRequest_init_zero;

  pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);
  if (!pb_decode(&stream, RofiRequest_fields, &rofi_request)) {
    ESP_LOGE(TAG, "Decoding failed: %s", PB_GET_ERROR(&stream));
    throw std::runtime_error("Decoding failed " +
                             std::string(PB_GET_ERROR(&stream)));
  }

  ESP_LOGI(TAG, "Received RoFI Request: Type=%d", rofi_request.command);

  switch (rofi_request.command) {
  case CommandTypeRequest_DEVICE:
    ESP_LOGI(TAG, "Device Command");
    encodeDeviceCommand(rofi_request.command_type.device);
    break;

  case CommandTypeRequest_JOINT:
    ESP_LOGI(TAG, "Joint Command");
    break;

  case CommandTypeRequest_CONNECTOR:
    ESP_LOGI(TAG, "Connector Command");
    break;
  }

  return rofi_request.packet_id;
}

size_t NimBleCodec::encodeRofiResponse(uint8_t *buffer, size_t buffer_size,
                                       bool is_success, int32_t packet_id,
                                       std::string message) {
  RofiResponse rofi_response = RofiResponse_init_zero;
  rofi_response.packet_id = packet_id;
  rofi_response.rofi_id = 1;
  rofi_response.success = is_success;

  // Set the message field callback
  rofi_response.message.arg = (void *)message.c_str();
  rofi_response.message.funcs.encode = &encode_string;

  pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_size);
  if (!pb_encode(&stream, RofiResponse_fields, &rofi_response)) {
    ESP_LOGE(TAG, "Encoding failed: %s", PB_GET_ERROR(&stream));
    throw std::runtime_error("Encoding failed");
  }

  // print binary message
  ESP_LOGI(TAG, "Encoded RoFI Response binary: ");
  for (size_t i = 0; i < stream.bytes_written; i++) {
    printf("%02X ", buffer[i]);
  }
  printf("\n");

  // print size of the message
  ESP_LOGI(TAG, "Encoded RoFI Response size: %d", stream.bytes_written);

  return stream.bytes_written;
}
} // namespace rofi::nimble