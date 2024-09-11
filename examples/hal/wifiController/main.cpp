#include <cmath>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <iostream>
#include <sstream>

#include <optional>
#include <memory>

#include <net/wifiController.hpp>
#include <nvs/nvs.hpp>
#include <util/delay.hpp>

#include <rofi_hal.hpp>

#define STATION_MODE 1 // 0 - AP mode, 1 - STA mode

std::optional<rofi::hal::RoFI> localRoFI;
std::unique_ptr<esp::EspWifiController> wifi;

extern "C" void app_main() {

  localRoFI = rofi::hal::RoFI::getLocalRoFI();

  wifi = std::unique_ptr<esp::EspWifiController>(&esp::EspWifiController::get());
  wifi->setMode(esp::EspWifiController::Mode::DISABLED);

  // print saved networks
  // not needed initialization - just read config from NVS
  std::cout << "Saved Networks:\n";
  auto networks = wifi->listWifiNetworks();
  for (auto &network : networks) {
    std::cout << "SSID: " << network << std::endl;
  }
  std::cout << "End of saved networks\n\n";


  std::cout << "Starting wifi\n";
  wifi->initialize();

  #if STATION_MODE

  // STA mode
  std::cout << "Setting mode to STATION, adding network based on SSID and PASSWORD loaded from ENV\n";
  wifi->setMode(esp::EspWifiController::Mode::STATION);
  wifi->setStaModeOption(esp::EspWifiController::StaMode::BEST_SIGNAL);
  wifi->addWifiNetwork(SSID, PASSWORD);

  while (wifi->currentIp().addr == 0) {
    std::cout << "Waiting for IP\n";
    esp::delayMs(1000);
  }
  std::cout << "IP: " << wifi->currentIpStr() << "\n";
  std::cout << "Connected to: " << wifi->currentStaSsid() << "\n\n";

  #else

  // AP mode
  esp::delayMs(1000);
  std::cout << "Setting mode to AP\n";
  wifi->setMode(esp::EspWifiController::Mode::AP);
  wifi->setApWifiNetwork("RoFI-AP", "RoFI-AP-Pass");

  while (wifi->currentIp().addr == 0) {
    std::cout << "Waiting for IP\n";
    esp::delayMs(1000);
  }

  std::cout << "IP: " << wifi->currentIpStr() << "\n\n";

  while (true) {
    auto clients = wifi->getConnectedClientsStr();
    std::cout << "Connected clients:\n";

    if (clients.empty()) {
      std::cout << " - no connected clients\n";
    } else {
      for (auto &client : clients) {
        std::cout << " - " << client.first << " - " << client.second << std::endl;
      }
    }

    std::cout << "End of connected clients (waiting 2s)\n\n";
    esp::delayMs(2000);
  }

  #endif

  std::cout << "End of main\n";
}
