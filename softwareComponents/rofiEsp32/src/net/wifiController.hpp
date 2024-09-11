#pragma once

#include "esp_netif.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include <cstring>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "nvs/nvs.hpp"

namespace esp {

// esp::EspWifiController exception
class EspWifiControllerException : public std::exception {
public:
  explicit EspWifiControllerException(const std::string &msg) : _msg(msg) {}
  const char *what() const noexcept override { return _msg.c_str(); }

private:
  std::string _msg;
};

class EspWifiController {
public:
  enum Mode : uint8_t {
    DISABLED,
    STATION,
    AP,

    MAX,
  };

  enum StaMode : uint8_t {
    // Connect to any known network, pick the one with better signal if multiple
    // found
    BEST_SIGNAL,
    // Connect to SSID specified in sta_ssid only
    SPECIFIC_SSID,
  };

  static constexpr const char *KvNsWifiSsid = "wifi_net";
  static constexpr const char *KvNsWifiMain = "wifi_cfg";
  static constexpr const char *KeyWifiMode = "mode";
  static constexpr const char *KeyWifiStaMode = "sta_mode";
  static constexpr const char *KeyWifiStaSpecific = "sta_ssid";
  static constexpr const char *KeyWifiStaApFallback = "sta_ap_fallback";
  static constexpr const char *KeyWifiApSsid = "ap_ssid";
  static constexpr const char *KeyWifiApPass = "ap_pass";
  static constexpr const char *KeyWifiCurrentIp =
      "current_ip"; // "fake" key that is actually only ever in memory

private:
  EspWifiController();
  EspWifiController(EspWifiController &) = delete;

  Mode _mode;
  StaMode _staMode;
  std::string _currentStaSsid;
  bool _apFallback;
  esp_ip4_addr_t _currentIp;

  bool _eventLoopOurs;
  esp_event_handler_instance_t _handler_wifi;
  esp_event_handler_instance_t _handler_ip;
  std::unique_ptr<esp_netif_t, decltype(&esp_netif_destroy_default_wifi)>
      _netIf;
  esp_timer_handle_t _kvModifyTimer;
  std::mutex _modeLock;

  bool initGlobalsLocked();
  void deinitGlobalsLocked();

  bool switchMode(Mode newMode, std::unique_ptr<EspNvs> mainNs = nullptr);
  static void onKvModifiedTimer(void *selfVoid);

  bool registerEventHandlersLocked();
  void unregisterEventHandlersLocked();
  void stopWifiLocked();

  static void eventHandlerWifi(void *selfVoid, esp_event_base_t event_base,
                               int32_t event_id, void *event_data);
  static void eventHandlerWifiStop(void *semVoid, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data);
  static void eventHandlerIp(void *selfVoid, esp_event_base_t event_base,
                             int32_t event_id, void *event_data);

public:
  static EspWifiController &get() {
    static EspWifiController instance;
    return instance;
  }

  bool initialize();

  void onKeyValueModified(const std::string &nsname, const std::string &key);

  esp_ip4_addr_t currentIp() const { return _currentIp; }
  std::string currentIpStr() const {
    char buf[16];
    esp_ip4addr_ntoa(&_currentIp, buf, sizeof(buf));
    return buf;
  }

  // Mode operations
  Mode mode() const { return _mode; }
  std::string modeStr();
  void setMode(Mode newMode);

  // Station mode
  void addWifiNetwork(const std::string &ssid, const std::string &pass);
  void removeWifiNetwork(const std::string &ssid);
  std::vector<std::string> listWifiNetworks();
  void setStaModeOption(StaMode mode);
  StaMode staModeOption() const { return _staMode; }
  std::string currentStaSsid() const { return _currentStaSsid; }

  // STA mode options
  void setApFallback(bool fallback);
  bool apFallback() const { return _apFallback; }

  // AP mode
  void setApWifiNetwork(const std::string &ssid, const std::string &pass);
  std::vector<esp_netif_pair_mac_ip_t> getConnectedClients();
  std::vector<std::pair<std::string, std::string>> getConnectedClientsStr();
};

} // namespace esp