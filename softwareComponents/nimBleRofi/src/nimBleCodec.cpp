#include "nimBleCodec.hpp"

#include "esp_log.h"
#include "esp_system.h"
#include "utils.hpp"
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

bool decode_string(pb_istream_t *stream, const pb_field_t *field, void **arg) {
  char *buffer =
      (char *)*arg; // Cast the arg to the appropriate type (char buffer)
  size_t size = stream->bytes_left; // Get the remaining size in the stream

  if (size > 255) { // Adjust if your buffer size is different
    size = 255;
  }

  if (!pb_read(stream, (pb_byte_t *)buffer, size)) {
    return false;
  }

  buffer[size] = '\0'; // Null-terminate the string

  return true;
}

bool encode_joints(pb_ostream_t *stream, const pb_field_t *field,
                   void *const *arg) {
  Joint *joints = (Joint *)(*arg); // Cast the arg to the appropriate type
  for (int i = 0; i < 3; i++) {    // Iterate over the joints array
    if (!pb_encode_tag_for_field(stream, field)) {
      return false;
    }
    if (!pb_encode_submessage(stream, Joint_fields, &joints[i])) {
      return false;
    }
  }
  return true;
}

bool encode_connectors(pb_ostream_t *stream, const pb_field_t *field,
                       void *const *arg) {
  Connector *connectors =
      (Connector *)(*arg);      // Cast the arg to the appropriate type
  for (int i = 0; i < 6; i++) { // Iterate over the connectors array
    if (!pb_encode_tag_for_field(stream, field)) {
      return false;
    }
    if (!pb_encode_submessage(stream, Connector_fields, &connectors[i])) {
      return false;
    }
  }
  return true;
}

size_t NimBleCodec::encodeRofiState(uint8_t *buffer, size_t buffer_size,
                                    uint32_t packet_id) {
  RofiState rofi_state = RofiState_init_zero;
  rofi_state.packet_id = packet_id;
  rofi_state.rofi_id = _localRoFI.getId();
  rofi_state.type = RofiStateType_STATE_SUCCESS;
  rofi_state.which_state_type = RofiState_state_data_tag;

  rofi_state.state_type.state_data.random_number = esp_random() % 1000;

  Descriptor descriptor = Descriptor_init_zero;
  ESP_LOGI(TAG, "RoFI Descriptor: JointCount=%d, ConnectorCount=%d",
           _localRoFI.getDescriptor().jointCount,
           _localRoFI.getDescriptor().connectorCount);

  descriptor.count_joint = _localRoFI.getDescriptor().jointCount;
  descriptor.count_connector = _localRoFI.getDescriptor().connectorCount;
  rofi_state.state_type.state_data.descriptor = descriptor;
  rofi_state.state_type.state_data.has_descriptor = true;

  // // Create 3 Joint instances
  // static Joint joints[3]; // Static to ensure they stay in memory
  // for (int i = 0; i < 3; i++) {
  //   joints[i] = Joint_init_zero;
  //   joints[i].max_position = _localRoFI.getJoint(i).maxPosition();
  //   joints[i].min_position = _localRoFI.getJoint(i).minPosition();
  //   joints[i].max_speed = _localRoFI.getJoint(i).maxSpeed();
  //   joints[i].min_speed = _localRoFI.getJoint(i).minSpeed();
  //   joints[i].max_torque = _localRoFI.getJoint(i).maxTorque();
  //   joints[i].velocity = _localRoFI.getJoint(i).getVelocity();
  //   joints[i].position = _localRoFI.getJoint(i).getPosition();
  //   joints[i].torque = _localRoFI.getJoint(i).getTorque();
  // }
  // rofi_state.state_type.state_data.joints.arg = joints;
  // rofi_state.state_type.state_data.joints.funcs.encode = &encode_joints;

  // Create 6 Connector instances
  static Connector connectors[6]; // Static to ensure they stay in memory
  for (int i = 0; i < 6; i++) {
    connectors[i] = Connector_init_zero;
    connectors[i].position = static_cast<ConnectorPosition>(
        _localRoFI.getConnector(i).getState().position);
    connectors[i].internal = _localRoFI.getConnector(i).getState().internal;
    connectors[i].external = _localRoFI.getConnector(i).getState().external;
    connectors[i].distance_mode = static_cast<LidarDistanceMode>(
        _localRoFI.getConnector(i).getState().distanceMode);
    // connectors[i].connected =
    // _localRoFI.getConnector(i).getState().connected;
    // connectors[i].orientation =
    //     _localRoFI.getConnector(i).getState().orientation;
    // connectors[i].internal_voltage =
    //     _localRoFI.getConnector(i).getState().internalVoltage;
    // connectors[i].internal_current =
    //     _localRoFI.getConnector(i).getState().internalCurrent;
    // connectors[i].external_voltage =
    //     _localRoFI.getConnector(i).getState().externalVoltage;
    // connectors[i].external_current =
    //     _localRoFI.getConnector(i).getState().externalCurrent;
    // connectors[i].lidar_status =
    //     _localRoFI.getConnector(i).getState().lidarStatus;
    // connectors[i].distance = _localRoFI.getConnector(i).getState().distance;
  }
  rofi_state.state_type.state_data.connectors.arg = connectors;
  rofi_state.state_type.state_data.connectors.funcs.encode = &encode_connectors;

  pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_size);
  if (!pb_encode(&stream, RofiState_fields, &rofi_state)) {
    std::string error_message =
        "Encoding failed: " + std::string(PB_GET_ERROR(&stream));
    ESP_LOGE(TAG, "%s", error_message.c_str());
    throw std::runtime_error(error_message);
  }

  // ESP_LOGI(TAG, "Encoded RoFI State: len=%d, data=%s", stream.bytes_written,
  //          buffer_to_hex(buffer, stream.bytes_written).c_str());

  return stream.bytes_written;
}

size_t NimBleCodec::encodeRofiStateError(uint8_t *buffer, size_t buffer_size,
                                         uint32_t packet_id,
                                         std::string error_message) {
  RofiState rofi_state = RofiState_init_zero;
  rofi_state.packet_id = packet_id;
  rofi_state.rofi_id = _localRoFI.getId();
  rofi_state.type = RofiStateType_STATE_ERROR;
  rofi_state.which_state_type = RofiState_error_message_tag;

  // if message length is too long, truncate it
  if (error_message.length() > 100) {
    error_message = error_message.substr(0, 100);
  }

  rofi_state.state_type.error_message.arg = (void *)error_message.c_str();
  rofi_state.state_type.error_message.funcs.encode = &encode_string;

  pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_size);
  if (!pb_encode(&stream, RofiState_fields, &rofi_state)) {
    std::string error_message =
        "Encoding failed: " + std::string(PB_GET_ERROR(&stream));
    ESP_LOGE(TAG, "%s", error_message.c_str());
    throw std::runtime_error(error_message);
  }

  return stream.bytes_written;
}

void NimBleCodec::encodeMessage(Message message) {
  // const int message_length = 256;
  //   char message_buffer[message_length];

  //   if (message.message.arg == nullptr) {
  //     ESP_LOGE(TAG, "Message is empty");
  //     return;
  //   }

  //   char *message_str = (char*)message.message.arg;
  //   snprintf(message_buffer, message_length, "%s", message_str);
  //   ESP_LOGI(TAG, "Message: %s", message_buffer);
  ;
}

void NimBleCodec::encodeDeviceCommand(DeviceCommand device) {
  switch (device.command) {
  case DeviceCommandType_REBOOT:
    ESP_LOGI(TAG, "Device Command: REBOOT");
    esp_restart();
    break;

  case DeviceCommandType_SET_ID:
    ESP_LOGI(TAG, "Device Command: SET_ID=%d", device.command_type.set_id);
    // _localRoFI.setId(device.command_type.set_id);
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
  case ConnectorCommandType_SET_CONNECT: {
    ESP_LOGI(TAG, "Connector (ID=%d) Command: SET_CONNECT",
             connector.connector_id);
    _localRoFI.getConnector(connector.connector_id).connect();
  }

  case ConnectorCommandType_SET_DISCONNECT: {
    ESP_LOGI(TAG, "Connector (ID=%d) Command: SET_DISCONNECT",
             connector.connector_id);
    _localRoFI.getConnector(connector.connector_id).disconnect();
  }

  case ConnectorCommandType_CONNECT_POWER: {
    ESP_LOGI(TAG, "Connector (ID=%d) Command: CONNECT_POWER=%d",
             connector.connector_id, connector.command_type.connect_power);
    if (connector.command_type.connect_power == ConnectorLine_INTERNAL) {
      _localRoFI.getConnector(connector.connector_id)
          .connectPower(rofi::hal::ConnectorLine::Internal);
    } else {
      _localRoFI.getConnector(connector.connector_id)
          .connectPower(rofi::hal::ConnectorLine::External);
    }
    break;
  }

  case ConnectorCommandType_DISCONNECT_POWER: {
    ESP_LOGI(TAG, "Connector (ID=%d) Command: DISCONNECT_POWER=%d",
             connector.connector_id, connector.command_type.disconnect_power);
    if (connector.command_type.disconnect_power == ConnectorLine_INTERNAL) {
      _localRoFI.getConnector(connector.connector_id)
          .disconnectPower(rofi::hal::ConnectorLine::Internal);
    } else {
      _localRoFI.getConnector(connector.connector_id)
          .disconnectPower(rofi::hal::ConnectorLine::External);
    }
    break;
  }

  case ConnectorCommandType_SET_DISTANCE_MODE: {
    ESP_LOGI(TAG, "Connector (ID=%d) Command: SET_DISTANCE_MODE=%d",
             connector.connector_id, connector.command_type.set_distance_mode);
    switch (connector.command_type.set_distance_mode) {
    case LidarDistanceMode_AUTONOMOUS:
      _localRoFI.getConnector(connector.connector_id)
          .setDistanceMode(rofi::hal::LidarDistanceMode::Autonomous);
      break;
    case LidarDistanceMode_SHORT:
      _localRoFI.getConnector(connector.connector_id)
          .setDistanceMode(rofi::hal::LidarDistanceMode::Short);
      break;
    case LidarDistanceMode_LONG:
      _localRoFI.getConnector(connector.connector_id)
          .setDistanceMode(rofi::hal::LidarDistanceMode::Long);
      break;
    }
    break;
  }
  }
}

int32_t NimBleCodec::decodeRofiRequest(const uint8_t *buffer,
                                       size_t message_length) {
  RofiRequest rofi_request = RofiRequest_init_zero;

  pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);
  if (!pb_decode(&stream, RofiRequest_fields, &rofi_request)) {
    std::string error_message =
        "Decoding failed: " + std::string(PB_GET_ERROR(&stream));
    ESP_LOGE(TAG, "%s", error_message.c_str());
    throw std::runtime_error(error_message);
  }

  ESP_LOGI(TAG, "Received RoFI Request: Type=%d", rofi_request.command);

  switch (rofi_request.command) {
  case CommandTypeRequest_MESSAGE:
    encodeMessage(rofi_request.command_type.message);
    break;

  case CommandTypeRequest_DEVICE:
    ESP_LOGI(TAG, "Device Command");
    encodeDeviceCommand(rofi_request.command_type.device);
    break;

  case CommandTypeRequest_JOINT:
    ESP_LOGI(TAG, "Joint Command");
    encodeJointCommand(rofi_request.command_type.joint);
    break;

  case CommandTypeRequest_CONNECTOR:
    ESP_LOGI(TAG, "Connector Command");
    encodeConnectorCommand(rofi_request.command_type.connector);
    break;
  }

  return rofi_request.packet_id;
}

size_t NimBleCodec::encodeRofiResponse(uint8_t *buffer, size_t buffer_size,
                                       int32_t packet_id, bool is_success,
                                       std::string message) {
  RofiResponse rofi_response = RofiResponse_init_zero;
  rofi_response.packet_id = packet_id;
  rofi_response.success = is_success;

  // if message length is too long, truncate it
  if (message.length() > 100) {
    message = message.substr(0, 100);
  }

  // Set the message field callback
  rofi_response.message.arg = (void *)message.c_str();
  rofi_response.message.funcs.encode = &encode_string;

  pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_size);
  if (!pb_encode(&stream, RofiResponse_fields, &rofi_response)) {
    std::string error_message =
        "Encoding failed: " + std::string(PB_GET_ERROR(&stream));
    ESP_LOGE(TAG, "%s", error_message.c_str());
    throw std::runtime_error(error_message);
  }
  return stream.bytes_written;
}
} // namespace rofi::nimble