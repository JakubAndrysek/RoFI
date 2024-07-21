#pragma once

#include "NimBLEDevice.h"
#include "NimBLELog.h"
#include "nimBleCodec.hpp"
// #include "nimBleRofi.hpp"
#include <optional>
#include <functional>

#define TAG "NIMBLE_CHARACTERISTIC"

namespace rofi::nimble {

/**
 * @file nimBleCharact.hpp
 * @brief Contains the class definitions for the MyServerCallbacks and
 * CharacteristicCallbacks classes.
 */
class RofiServerCallbacks : public NimBLEServerCallbacks {
private:

  NimBLEServer *_pServer;

  // void (*_connectionUpdate)(bool);
  std::function<void(bool)> _connectionUpdate;

  void onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo);

  void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo,
                    int reason);

  void onMTUChange(uint16_t MTU, NimBLEConnInfo &connInfo);

  uint32_t onPassKeyDisplay();

  void onConfirmPIN(const NimBLEConnInfo &connInfo, uint32_t pass_key);

  void onAuthenticationComplete(const NimBLEConnInfo &connInfo);

public:
  RofiServerCallbacks(
    const std::function<void(bool)> &cb,
    NimBLEServer *pServer);
};

/**
 * @brief  Callbacks for the NimBLECharacteristic.
 */
class CharactCallbacksRofiState : public NimBLECharacteristicCallbacks {
private:
  NimBleCodec _nimBleCodec;
  std::optional<rofi::hal::RoFI> _localRoFI;
  std::function<void(uint16_t)> _subscribeRofiStateUpdate;

  void onRead(NimBLECharacteristic *pCharacRofiState, NimBLEConnInfo &connInfo);

  void onWrite(NimBLECharacteristic *pCharacRofiState,
               NimBLEConnInfo &connInfo);

  /** Called before notification or indication is sent,
   *  the value can be changed here before sending if desired.
   */
  void onNotify(NimBLECharacteristic *pCharacRofiState);

  /**
   *  The value returned in code is the NimBLE host return code.
   */
  void onStatus(NimBLECharacteristic *pCharacRofiState, int code);

  void onSubscribe(NimBLECharacteristic *pCharacRofiState,
                   NimBLEConnInfo &connInfo, uint16_t subValue);

public:
  CharactCallbacksRofiState(NimBleCodec nimBleCodec, rofi::hal::RoFI localRoFI,
                            std::function<void(uint16_t)> subCb);
};

/**
 * @brief  Callbacks for the NimBLECharacteristic.
 */
class CharactCallbacksRofiCommand : public NimBLECharacteristicCallbacks {
private:
  NimBleCodec _nimBleCodec;
  std::optional<rofi::hal::RoFI> _localRoFI;

  void onRead(NimBLECharacteristic *pCharacRofiCommand,
              NimBLEConnInfo &connInfo);

  void onWrite(NimBLECharacteristic *pCharacRofiCommand,
               NimBLEConnInfo &connInfo);

public:
  CharactCallbacksRofiCommand(NimBleCodec nimBleCodec,
                              rofi::hal::RoFI localRoFI);
};

} // namespace rofi::nimble