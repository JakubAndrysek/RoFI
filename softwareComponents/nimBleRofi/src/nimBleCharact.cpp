#include "nimBleCharact.hpp"
#include <iostream>

// Callbacks for the NimBLEServer.
//////////////////////////////////////////

namespace rofi::nimble {

// std::function<void(bool)> _connectionUpdate;
RofiServerCallbacks::RofiServerCallbacks(const std::function<void(bool)> &cb,
                                         NimBLEServer *pServer)
    : _connectionUpdate(cb), _pServer(pServer) {}

void RofiServerCallbacks::onConnect(NimBLEServer *pServer,
                                    NimBLEConnInfo &connInfo) {
  ESP_LOGI(TAG_C, "Client address: %s\n",
           connInfo.getAddress().toString().c_str());

  _connectionUpdate(true);

  /** We can use the connection handle here to ask for different connection
   * parameters. Args: connection handle, min connection interval, max
   * connection interval latency, supervision timeout. Units; Min/Max
   * Intervals: 1.25 millisecond increments. Latency: number of intervals
   * allowed to skip. Timeout: 10 millisecond increments, try for 3x interval
   * time for best results.
   */
  pServer->updateConnParams(connInfo.getConnHandle(), 24, 48, 0, 18);
};

void RofiServerCallbacks::onDisconnect(NimBLEServer *pServer,
                                       NimBLEConnInfo &connInfo, int reason) {
  ESP_LOGI(TAG_C, "Client disconnected - start advertising");
  _connectionUpdate(false);

  NimBLEDevice::startAdvertising();
};

void RofiServerCallbacks::onMTUChange(uint16_t MTU, NimBLEConnInfo &connInfo) {
  ESP_LOGI(TAG_C, "MTU updated: %u for connection ID: %u", MTU,
           connInfo.getConnHandle());
  _pServer->updateConnParams(connInfo.getConnHandle(), 24, 48, 0, 60);
};

/********************* Security handled here **********************
****** Note: these are the same return values as defaults ********/
uint32_t RofiServerCallbacks::onPassKeyDisplay() {
  ESP_LOGI(TAG_C, "Server Passkey Display");
  /** This should return a random 6 digit number for security
   *  or make your own static passkey as done here.
   */
  return 123456;
};

void RofiServerCallbacks::onConfirmPIN(const NimBLEConnInfo &connInfo,
                                       uint32_t pass_key) {
  ESP_LOGI(TAG_C, "The passkey YES/NO number: %u", pass_key);
  /** Inject false if passkeys don't match. */
  NimBLEDevice::injectConfirmPIN(connInfo, true);
};

void RofiServerCallbacks::onAuthenticationComplete(
    const NimBLEConnInfo &connInfo) {
  /** Check that encryption was successful, if not we disconnect the client */
  if (!connInfo.isEncrypted()) {
    NimBLEDevice::getServer()->disconnect(connInfo.getConnHandle());
    printf("Encrypt connection failed - disconnecting client\n");
    return;
  }
  printf("Starting BLE work!");
};

// Callbacks for the NimBLECharacteristic.
//////////////////////////////////////////

CharactCallbacksRofiState::CharactCallbacksRofiState(
    NimBleCodec nimBleCodec, rofi::hal::RoFI localRoFI,
    std::function<void(uint16_t)> subCb)
    : _nimBleCodec(nimBleCodec), _localRoFI(localRoFI),
      _subscribeRofiStateUpdate(subCb) {}

void CharactCallbacksRofiState::onRead(NimBLECharacteristic *pCharacRofiState,
                                       NimBLEConnInfo &connInfo) {
  ESP_LOGD(TAG_C, "onRead() - RoFI State");

  static uint32_t packet_id = 1;
  uint8_t buffer[800];

  try {
    size_t message_length =
        _nimBleCodec.encodeRofiState(buffer, sizeof(buffer), packet_id);

    if (message_length == 0) {
      ESP_LOGE(TAG_C, "Encoding RofiState - onRead() message failed");
    }

    packet_id++;
    pCharacRofiState->setValue((uint8_t *)buffer, message_length);
    pCharacRofiState->notify();
  } catch (const std::runtime_error &e) {
    ESP_LOGE(TAG_C, "Encoding RofiState - onRead() message failed");
    return;
  }
}

void CharactCallbacksRofiState::onWrite(NimBLECharacteristic *pCharacRofiState,
                                        NimBLEConnInfo &connInfo) {
  NimBLEAttValue value = pCharacRofiState->getValue();
  ESP_LOGD(TAG_C, "Processing RoFI State (size: %d)", value.length());
}

/** Called before notification or indication is sent,
 *  the value can be changed here before sending if desired.
 */
void CharactCallbacksRofiState::onNotify(
    NimBLECharacteristic *pCharacRofiState) {
  ESP_LOGD(TAG_C, "onNotify() - RoFI State");
}

/**
 *  The value returned in code is the NimBLE host return code.
 */
void CharactCallbacksRofiState::onStatus(NimBLECharacteristic *pCharacRofiState,
                                         int code) {
  ESP_LOGI(TAG_C, "Notification/Indication return code: %d, %s - RoFI State",
           NimBLEUtils::returnCodeToString(code));
}

void CharactCallbacksRofiState::onSubscribe(
    NimBLECharacteristic *pCharacRofiState, NimBLEConnInfo &connInfo,
    uint16_t subValue) {

  ESP_LOGD(TAG_C, "onSubscribe() type: %d - RoFI State", subValue);
  if (subValue == 0) {
    ESP_LOGV(TAG_C, "Unsubscribed");
  } else if (subValue == 1) {
    ESP_LOGV(TAG_C, "Notification enabled");
  } else if (subValue == 2) {
    ESP_LOGV(TAG_C, "Indication enabled");
  }

  _subscribeRofiStateUpdate(subValue);
}

// Callbacks for the NimBLECharacteristic.
//////////////////////////////////////////

CharactCallbacksRofiCommand::CharactCallbacksRofiCommand(
    NimBleCodec nimBleCodec, rofi::hal::RoFI localRoFI)
    : _nimBleCodec(nimBleCodec), _localRoFI(localRoFI) {}

void CharactCallbacksRofiCommand::onRead(
    NimBLECharacteristic *pCharacRofiCommand, NimBLEConnInfo &connInfo) {
  ESP_LOGD(TAG_C, "onRead() - RoFI Command");
}

void CharactCallbacksRofiCommand::onWrite(
    NimBLECharacteristic *pCharacRofiCommand, NimBLEConnInfo &connInfo) {
  NimBLEAttValue value = pCharacRofiCommand->getValue();
  ESP_LOGD(TAG_C, "Processing RoFI Request (size: %d) - RoFI Command",
           value.length());

  bool success = true;
  int32_t packet_id = 1;
  std::string message = "Response OK";
  try {
    packet_id = _nimBleCodec.decodeRofiRequest(value.data(), value.length());
  } catch (const std::exception &e) {
    success = false;
    std::cerr << e.what() << '\n';
    message = "Response failed: " + std::string(e.what());
    // if message length is too long, truncate it
    if (message.length() > 100) {
      message = message.substr(0, 100);
    }
  }

  uint8_t buffer[600];
  size_t buffer_size = _nimBleCodec.encodeRofiResponse(
      buffer, sizeof(buffer), packet_id + 1, success, message);
  pCharacRofiCommand->setValue(buffer, buffer_size);
  pCharacRofiCommand->notify();
}

} // namespace rofi::nimble