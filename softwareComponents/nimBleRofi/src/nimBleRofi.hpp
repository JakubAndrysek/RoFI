#pragma once

#include "NimBLEDevice.h"
#include "nimBleCharact.hpp"
#include "protoc/rofi.pb.h"
#include <memory>
#include <optional>
#include <rofi_hal.hpp>

#define TAG "NIMBLE_ROFI"

namespace rofi::nimble {

class NimBleRofi {
private:

  enum NimBleState {
    UNSUBSCRIBED = 0,
    NOTIFICATION_ENABLED = 1,
    INDICATION_ENABLED = 2
  } _nimBleState;

  NimBleCodec _nimBleCodec;

  std::optional<rofi::hal::RoFI> _localRoFI;

  NimBLEServer *_pServer;
  NimBLEService *_pService;
  NimBLECharacteristic *_pCharacRofiState;
  NimBLECharacteristic *_pCharacRofiCommand;
  NimBLEAdvertising *_pAdvertising;

  CharactCallbacksRofiState _chrCallbacksRofiState;
  CharactCallbacksRofiCommand _chrCallbacksRofiCommand;
  NimBleState _rofiState = UNSUBSCRIBED;

  RofiServerCallbacks _rofiServerCallbacks;

  bool _deviceConnected = false;
  bool _oldDeviceConnected = false;

  static void notificatonTask(void *parameter);

  TaskHandle_t _notificationTaskHandle; // Add this line
  bool _stopTask; // Add this line to signal the task to stop

public:
  NimBleRofi(rofi::hal::RoFI localRoFI);
  ~NimBleRofi();
  void start();
  void stop();
  void updateRofiState();

  void connectionUpdate(bool connected);
  void subscribeRofiStateUpdate(uint16_t subValue);
};
} // namespace rofi::nimble