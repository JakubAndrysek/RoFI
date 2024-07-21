#include "nimBleCharact.hpp"

// Callbacks for the NimBLEServer.
//////////////////////////////////////////

namespace rofi::nimble {

// std::function<void(bool)> _connectionUpdate;
RofiServerCallbacks::RofiServerCallbacks(const std::function<void(bool)> &cb,
                                         NimBLEServer *pServer)
    : _connectionUpdate(cb), _pServer(pServer) {}

void RofiServerCallbacks::onConnect(NimBLEServer *pServer,
                                    NimBLEConnInfo &connInfo) {
  printf("Client address: %s\n", connInfo.getAddress().toString().c_str());

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
  printf("Client disconnected - start advertising\n");
  _connectionUpdate(false);

  NimBLEDevice::startAdvertising();
};

void RofiServerCallbacks::onMTUChange(uint16_t MTU, NimBLEConnInfo &connInfo) {
  printf("MTU updated: %u for connection ID: %u\n", MTU,
         connInfo.getConnHandle());
  _pServer->updateConnParams(connInfo.getConnHandle(), 24, 48, 0, 60);
};

/********************* Security handled here **********************
****** Note: these are the same return values as defaults ********/
uint32_t RofiServerCallbacks::onPassKeyDisplay() {
  printf("Server Passkey Display\n");
  /** This should return a random 6 digit number for security
   *  or make your own static passkey as done here.
   */
  return 123456;
};

void RofiServerCallbacks::onConfirmPIN(const NimBLEConnInfo &connInfo,
                                       uint32_t pass_key) {
  printf("The passkey YES/NO number: %" PRIu32 "\n", pass_key);
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
  printf("%s : onRead(), value: %s\n",
         pCharacRofiState->getUUID().toString().c_str(),
         pCharacRofiState->getValue().c_str());
}

void CharactCallbacksRofiState::onWrite(NimBLECharacteristic *pCharacRofiState,
                                        NimBLEConnInfo &connInfo) {
  printf("%s : onWrite(), value: %s\n",
         pCharacRofiState->getUUID().toString().c_str(),
         pCharacRofiState->getValue().c_str());

  printf("Characteristic written\n");
  ESP_LOGI(TAG, "Characteristic written");
  std::string value = pCharacRofiState->getValue();
  ESP_LOGI(TAG, "Data received: %s", value.c_str());
  //   decodeSensorData((const uint8_t *)value.c_str(), value.length());
}

/** Called before notification or indication is sent,
 *  the value can be changed here before sending if desired.
 */
void CharactCallbacksRofiState::onNotify(
    NimBLECharacteristic *pCharacRofiState) {
  printf("Sending notification to clients\n");
}

/**
 *  The value returned in code is the NimBLE host return code.
 */
void CharactCallbacksRofiState::onStatus(NimBLECharacteristic *pCharacRofiState,
                                         int code) {
  printf("Notification/Indication return code: %d, %s\n", code,
         NimBLEUtils::returnCodeToString(code));
}

void CharactCallbacksRofiState::onSubscribe(
    NimBLECharacteristic *pCharacRofiState, NimBLEConnInfo &connInfo,
    uint16_t subValue) {

  ESP_LOGI(TAG, "onSubscribe() type: %d", subValue);
  if (subValue == 0) {
    ESP_LOGI(TAG, "Unsubscribed");
  } else if (subValue == 1) {
    ESP_LOGI(TAG, "Notification enabled");
  } else if (subValue == 2) {
    ESP_LOGI(TAG, "Indication enabled");
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
  printf("%s : onRead(), value: %s\n",
         pCharacRofiCommand->getUUID().toString().c_str(),
         pCharacRofiCommand->getValue().c_str());
}

void CharactCallbacksRofiCommand::onWrite(
    NimBLECharacteristic *pCharacRofiCommand, NimBLEConnInfo &connInfo) {
  printf("%s : onWrite(), value: %s\n",
         pCharacRofiCommand->getUUID().toString().c_str(),
         pCharacRofiCommand->getValue().c_str());

  printf("Characteristic written\n");
  ESP_LOGI(TAG, "Characteristic written");
  std::string value = pCharacRofiCommand->getValue();
  ESP_LOGI(TAG, "Data received: %s", value.c_str());
  ESP_LOGI(TAG, "Decoding RoFI Request");
  int32_t packet_id = _nimBleCodec.decodeRofiRequest(
      (const uint8_t *)value.c_str(), value.length());

  uint8_t buffer[400];
  size_t buffer_size = _nimBleCodec.encodeRofiResponse(
      buffer, sizeof(buffer), true, packet_id, "Response OK");
  pCharacRofiCommand->setValue(buffer, buffer_size);
  pCharacRofiCommand->notify();
}

} // namespace rofi::nimble