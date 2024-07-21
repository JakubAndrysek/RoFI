#include "nimBleRofi.hpp"
#include "esp_random.h"
#include "nimBleCodec.hpp"
#include "pb_decode.h"
#include "pb_encode.h"

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
  NimBLEDevice::init("RoFI-" + std::to_string(_localRoFI->getId()));

  _pServer = NimBLEDevice::createServer();
  // _pServer->setCallbacks(new ServerCallbacks(_pServer));
  _pServer->setCallbacks(&_rofiServerCallbacks);

  _pService = _pServer->createService("A0F1");

  _pCharacRofiState = _pService->createCharacteristic(
      "D0F1",
      NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);

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
  xTaskCreate(NimBleRofi::notificatonTask, "notificationTask", 4096, this, 5,
              &_notificationTaskHandle);

  ESP_LOGI(TAG, "Advertising started!");
}

void NimBleRofi::notificatonTask(void *parameter) {
  NimBleRofi *nimBleRofi = static_cast<NimBleRofi *>(parameter);

  // Start the task loop
  while (!nimBleRofi->_stopTask) {
    // Run if the device is connected
    if (nimBleRofi->_deviceConnected && nimBleRofi->_rofiState == NOTIFICATION_ENABLED) {
      nimBleRofi->updateRofiState();
      vTaskDelay(2000 / portTICK_PERIOD_MS);
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
  static uint32_t packet_id = 0;
  uint8_t buffer[128];
  size_t message_length =
      _nimBleCodec.encodeRofiState(buffer, sizeof(buffer), packet_id++,
                                   _localRoFI->getId(), esp_random() % 100);

  if (message_length == 0) {
    ESP_LOGE(TAG, "Failed to encode sensor data");
    return;
  }

  _pCharacRofiState->setValue((uint8_t *)buffer, message_length);
  _pCharacRofiState->notify();

  // ESP_LOGI(TAG, "RoFI state updated");
}

void NimBleRofi::connectionUpdate(bool connected) {
  _deviceConnected = connected;
}

void NimBleRofi::subscribeRofiStateUpdate(uint16_t subValue) {
  _rofiState = static_cast<NimBleState>(subValue);
}

} // namespace rofi::nimble