#include "nimBleRofi.hpp"
#include "esp_random.h"
#include "nimBleCodec.hpp"
#include "pb_decode.h"
#include "pb_encode.h"
#include "utils.hpp"
#include <string.h>

namespace rofi::nimble {

NimBleRofi::NimBleRofi(rofi::hal::RoFI localRoFI)
    : _nimBleCodec(localRoFI), _localRoFI(localRoFI),
      _chrCallbacksRofiCommand(_nimBleCodec, localRoFI),
      _chrCallbacksRofiState(_nimBleCodec, localRoFI,
                             [this](uint16_t subValue) {
                               this->subscribeRofiStateUpdate(subValue);
                             }),
      _rofiServerCallbacks(
          [this](bool connected) { this->connectionUpdate(connected); },
          _pServer),
      _deviceConnected(false), _oldDeviceConnected(false),
      _notificationTaskHandle(nullptr), _stopTask(false) {}

NimBleRofi::~NimBleRofi() { stop(); }

void NimBleRofi::start() {
  const int id = _localRoFI->getId();
  std::string mac = getMACAddress();
  std::string deviceName;

  deviceName = "RoFI-" + std::to_string(id);

  // if (id == 0) {
  //   deviceName = "RoFI-" + mac;
  // } else {
  //   deviceName = "RoFI-" + std::to_string(id);
  // }

  NimBLEDevice::init(deviceName);
  ESP_LOGI(TAG, "NimBLEDevice initialized: %s", deviceName.c_str());

  _pServer = NimBLEDevice::createServer();
  _pServer->setCallbacks(&_rofiServerCallbacks);

  _pService = _pServer->createService("A0F1");

  _pCharacRofiState = _pService->createCharacteristic(
      "D0F1", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);

  _pCharacRofiState->setCallbacks(&_chrCallbacksRofiState);

  _pCharacRofiCommand = _pService->createCharacteristic(
      "D0F2", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);

  _pCharacRofiCommand->setCallbacks(&_chrCallbacksRofiCommand);

  _pService->start();

  _pAdvertising = NimBLEDevice::getAdvertising();
  _pAdvertising->addServiceUUID(_pService->getUUID());
  _pAdvertising->setScanResponse(true);
  _pAdvertising->start();

  _stopTask = false;
  xTaskCreate(NimBleRofi::notificatonTask, "notificationTask", 8192, this, 5,
              &_notificationTaskHandle);

  ESP_LOGI(TAG, "Advertising started!");
}

void NimBleRofi::notificatonTask(void *parameter) {
  NimBleRofi *nimBleRofi = static_cast<NimBleRofi *>(parameter);

  // Start the task loop
  while (!nimBleRofi->_stopTask) {
    // Run if the device is connected
    if (nimBleRofi->_deviceConnected &&
        nimBleRofi->_rofiState == NOTIFICATION_ENABLED) {
      nimBleRofi->updateRofiState();
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    // Disconnecting
    if (!nimBleRofi->_deviceConnected && nimBleRofi->_oldDeviceConnected) {
      // do stuff here on disconnecting
      nimBleRofi->_oldDeviceConnected = nimBleRofi->_deviceConnected;
    }

    // Connecting
    if (nimBleRofi->_deviceConnected && !nimBleRofi->_oldDeviceConnected) {
      // do stuff here on connecting
      nimBleRofi->_oldDeviceConnected = nimBleRofi->_deviceConnected;
    }

    // Delay between loops to reset watchdog timer
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

  vTaskDelete(NULL); // Delete this task
}

void NimBleRofi::stop() {
  _stopTask = true; // Signal the task to stop
  if (_notificationTaskHandle != NULL) {
    vTaskDelete(_notificationTaskHandle); // Delete the task
    _notificationTaskHandle = NULL;       // Reset the handle
  }

  _pAdvertising->stop();
  _pService->removeCharacteristic(_pCharacRofiState);
  _pService->removeCharacteristic(_pCharacRofiCommand);
  _pServer->removeService(_pService);
  // NimBLEDevice::deinit(); // This is causing a crash
}

void NimBleRofi::updateRofiState() {
  static uint32_t packet_id = 1;
  uint8_t buffer[400];

  try {
    size_t message_length =
        _nimBleCodec.encodeRofiState(buffer, sizeof(buffer), packet_id);
    if (message_length == 0) {
      throw std::runtime_error(
          "Encoding RofiState message failed - message_length == 0");
    }

    packet_id++;
    _pCharacRofiState->setValue((uint8_t *)buffer, message_length);
    _pCharacRofiState->notify();
  } catch (const std::runtime_error &e) {
    updateRofiStateError(packet_id, e.what());
  }
}

void NimBleRofi::updateRofiStateError(uint32_t &packet_id,
                                      std::string error_message) {
  uint8_t buffer_error[400];
  ESP_LOGE(TAG, "%s", error_message.c_str());

  size_t message_length_err = 0;
  try {
    message_length_err = _nimBleCodec.encodeRofiStateError(
        buffer_error, sizeof(buffer_error), packet_id, error_message);

    if (message_length_err == 0) {
      throw std::runtime_error("Encoding RofiStateError message failed");
    }
  } catch (const std::runtime_error &e) {
    std::string error_message = e.what();
    ESP_LOGE(TAG, "%s", error_message.c_str());
    throw e;
  }

  packet_id++;
  _pCharacRofiState->setValue((uint8_t *)buffer_error, message_length_err);
  _pCharacRofiState->notify();
}

void NimBleRofi::connectionUpdate(bool connected) {
  _deviceConnected = connected;
}

void NimBleRofi::subscribeRofiStateUpdate(uint16_t subValue) {
  _rofiState = static_cast<NimBleState>(subValue);
}

} // namespace rofi::nimble