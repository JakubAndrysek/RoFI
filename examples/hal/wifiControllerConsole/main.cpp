#include <driver/uart.h>
#include <esp_console.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <sdkconfig.h>

#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string_view>

#include <atoms/units.hpp>
#include <atoms/util.hpp>

#include <net/wifiController.hpp>
#include <nvs/nvs.hpp>
#include <rofi_hal.hpp>

using namespace std::literals;

// This is a simple utility, so let's make things simple and use global objects
// so we can reference it from plain C functions required by CLI interface
std::optional<rofi::hal::RoFI> localRoFI;

std::unique_ptr<esp::EspWifiController> wifi;

// Simple wrapper that catches any exceptions, reports them an returns with a
// non-zero code if any error ocurred.
template <void (*Proc)(int argc, char **argv)>
int handled(int argc, char **argv) {
  try {
    Proc(argc, argv);
  } catch (const std::exception &e) {
    std::cerr << "Failed: " << e.what() << "\n";
    return 1;
  }
  return 0;
}

void ensureArguments(int argc, int expected) {
  if (argc != expected + 1)
    throw std::runtime_error("Invalid number of arguments " +
                             std::to_string(argc));
}

////////////////////////// WIFI - MODE //////////////////////////

void getMode(int argc, char **argv) {
  ensureArguments(argc, 0);

  std::cout << "Current mode: " << (int)wifi->mode() << " - ";
  switch (wifi->mode()) {
  case esp::EspWifiController::Mode::DISABLED:
    std::cout << "Disabled" << std::endl;
    break;
  case esp::EspWifiController::Mode::STATION:
    std::cout << "Station" << std::endl;
    break;
  case esp::EspWifiController::Mode::AP:
    std::cout << "AP" << std::endl;
    break;
  default:
    std::cout << "Unknown" << std::endl;
    break;
  }
}

void registerGetMode() {
  const esp_console_cmd_t cmd = {.command = "getMode",
                                 .help = "Get current mode",
                                 .hint = "",
                                 .func = &handled<getMode>,
                                 .argtable = nullptr};
  ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

void setMode(int argc, char **argv) {
  ensureArguments(argc, 1);

  if (std::string(argv[1]).empty()) {
    throw std::runtime_error("Argument cannot be empty");
  }

  if (std::string(argv[1]) == "disabled") {
    wifi->setMode(esp::EspWifiController::Mode::DISABLED);
  } else if (std::string(argv[1]) == "station") {
    wifi->setMode(esp::EspWifiController::Mode::STATION);
  } else if (std::string(argv[1]) == "ap") {
    wifi->setMode(esp::EspWifiController::Mode::AP);
  } else {
    throw std::runtime_error(
        "Argument must be either string 'disabled' or 'station' or 'ap'");
  }
}

void registerSetMode() {
  const esp_console_cmd_t cmd = {.command = "setMode",
                                 .help = "Set mode",
                                 .hint = "<disabled|station|ap>",
                                 .func = &handled<setMode>,
                                 .argtable = nullptr};
  ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

////////////////////////// WIFI - IP //////////////////////////

void getIp(int argc, char **argv) {
  ensureArguments(argc, 0);

  std::cout << "Current IP: " << wifi->currentIpStr() << std::endl;
}

void registerGetIp() {
  const esp_console_cmd_t cmd = {.command = "getIp",
                                 .help = "Get current IP",
                                 .hint = "",
                                 .func = &handled<getIp>,
                                 .argtable = nullptr};
  ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

void waitForIp(int argc, char **argv) {
  ensureArguments(argc, 0);

  while (wifi->currentIp().addr == 0) {
    std::cout << "Waiting for IP..." << std::endl;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  std::cout << "IP acquired: " << wifi->currentIpStr() << std::endl;
}

void registerWaitForIp() {
  const esp_console_cmd_t cmd = {.command = "waitForIp",
                                 .help = "Wait for IP",
                                 .hint = "",
                                 .func = &handled<waitForIp>,
                                 .argtable = nullptr};
  ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

////////////////////////// WIFI - STA MODE //////////////////////////

void addWifiNetwork(int argc, char **argv) {
  ensureArguments(argc, 2);

  if (std::string(argv[1]).empty()) {
    throw std::runtime_error("SSID cannot be empty");
  }

  if (std::string(argv[2]).size() < 8) {
    throw std::runtime_error("Password must be at least 8 characters long");
  }

  wifi->addWifiNetwork(argv[1], argv[2]);
}

void registerAddWifiNetwork() {
  const esp_console_cmd_t cmd = {.command = "addWifiNetwork",
                                 .help = "Add a new wifi network",
                                 .hint = "<SSID> <password>",
                                 .func = &handled<addWifiNetwork>,
                                 .argtable = nullptr};
  ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

void removeWifiNetwork(int argc, char **argv) {
  ensureArguments(argc, 1);

  if (std::string(argv[1]).empty()) {
    throw std::runtime_error("SSID cannot be empty");
  }

  wifi->removeWifiNetwork(argv[1]);
}

void registerRemoveWifiNetwork() {
  const esp_console_cmd_t cmd = {.command = "removeWifiNetwork",
                                 .help = "Remove a wifi network",
                                 .hint = "<SSID>",
                                 .func = &handled<removeWifiNetwork>,
                                 .argtable = nullptr};
  ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

void listWifiNetworks(int argc, char **argv) {
  ensureArguments(argc, 0);

  auto networks = wifi->listWifiNetworks();
  if (networks.empty()) {
    std::cout << "No saved networks" << std::endl;
    return;
  }

  for (const auto &network : networks) {
    std::cout << network << std::endl;
  }
}

void registerListWifiNetworks() {
  const esp_console_cmd_t cmd = {.command = "listWifiNetworks",
                                 .help = "List all wifi networks",
                                 .hint = "",
                                 .func = &handled<listWifiNetworks>,
                                 .argtable = nullptr};
  ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

void setStaModeOption(int argc, char **argv) {
  ensureArguments(argc, 1);

  if (std::string(argv[1]).empty()) {
    throw std::runtime_error("Argument cannot be empty");
  }

  if (std::string(argv[1]) == "bestSignal") {
    wifi->setStaModeOption(esp::EspWifiController::StaMode::BEST_SIGNAL);
  } else if (std::string(argv[1]) == "specificSsid") {
    wifi->setStaModeOption(esp::EspWifiController::StaMode::SPECIFIC_SSID);
  } else {
    throw std::runtime_error(
        "Argument must be either string 'bestSignal' or 'specificSsid'");
  }
}

void registerSetStaModeOption() {
  const esp_console_cmd_t cmd = {.command = "setStaModeOption",
                                 .help = "Set STA mode option",
                                 .hint = "<bestSignal|specificSsid>",
                                 .func = &handled<setStaModeOption>,
                                 .argtable = nullptr};
  ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

void getStaModeOption(int argc, char **argv) {
  ensureArguments(argc, 0);

  std::cout << "STA mode option: ";
  switch (wifi->staModeOption()) {
  case esp::EspWifiController::StaMode::BEST_SIGNAL:
    std::cout << "Best signal" << std::endl;
    break;
  case esp::EspWifiController::StaMode::SPECIFIC_SSID:
    std::cout << "Specific SSID" << std::endl;
    break;
  default:
    std::cout << "Unknown" << std::endl;
    break;
  }
}

void registerGetStaModeOption() {
  const esp_console_cmd_t cmd = {.command = "getStaModeOption",
                                 .help = "Get STA mode option",
                                 .hint = "",
                                 .func = &handled<getStaModeOption>,
                                 .argtable = nullptr};
  ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

void currentStaSsid(int argc, char **argv) {
  ensureArguments(argc, 0);

  if (wifi->mode() != esp::EspWifiController::Mode::STATION) {
    throw std::runtime_error("Connected SSID is only available in STA mode");
  }

  std::string ssid = wifi->currentStaSsid();
  if (ssid.empty()) {
    std::cout << "Not connected to any network" << std::endl;
  } else {
    std::cout << "Connected to: " << ssid << std::endl;
  }
}

void registerConnectedSsid() {
  const esp_console_cmd_t cmd = {.command = "currentStaSsid",
                                 .help = "Get connected SSID",
                                 .hint = "",
                                 .func = &handled<currentStaSsid>,
                                 .argtable = nullptr};
  ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

////////////////////////// WIFI - STA OPTION //////////////////////////

void setApFallback(int argc, char **argv) {
  ensureArguments(argc, 1);

  if (std::string(argv[1]).empty()) {
    throw std::runtime_error("Argument cannot be empty");
  }

  if (std::string(argv[1]) == "true") {
    wifi->setApFallback(true);
  } else if (std::string(argv[1]) == "false") {
    wifi->setApFallback(false);
  } else {
    throw std::runtime_error(
        "Argument must be either string 'true' or 'false'");
  }
}

void registerSetApFallback() {
  const esp_console_cmd_t cmd = {.command = "setApFallback",
                                 .help = "Set AP fallback",
                                 .hint = "<true|false>",
                                 .func = &handled<setApFallback>,
                                 .argtable = nullptr};
  ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

void getApFallback(int argc, char **argv) {
  ensureArguments(argc, 0);

  std::cout << "AP fallback: " << (wifi->apFallback() ? "true" : "false")
            << std::endl;
}

void registerGetApFallback() {
  const esp_console_cmd_t cmd = {.command = "getApFallback",
                                 .help = "Get AP fallback",
                                 .hint = "",
                                 .func = &handled<getApFallback>,
                                 .argtable = nullptr};
  ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

////////////////////////// WIFI - AP MODE //////////////////////////

void setAp(int argc, char **argv) {
  ensureArguments(argc, 2);

  if (std::string(argv[1]).empty()) {
    throw std::runtime_error("SSID cannot be empty");
  }

  if (std::string(argv[2]).size() < 8) {
    throw std::runtime_error("Password must be at least 8 characters long");
  }

  wifi->setMode(esp::EspWifiController::Mode::AP);
  wifi->setApWifiNetwork(argv[1], argv[2]);
}

void registerSetAp() {
  const esp_console_cmd_t cmd = {.command = "setAp",
                                 .help = "Set AP SSID and password",
                                 .hint = "<SSID> <password>",
                                 .func = &handled<setAp>,
                                 .argtable = nullptr};
  ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

void getConnectedClientsStr(int argc, char **argv) {
  if (wifi->mode() != esp::EspWifiController::Mode::AP) {
    throw std::runtime_error("Client list is only available in AP mode");
  }

  auto clients = wifi->getConnectedClientsStr();
  std::cout << "Connected clients:\n";

  if (clients.empty()) {
    std::cout << " - no connected clients\n";
  } else {
    for (const auto &client : clients) {
      std::cout << " - " << client.first << " - " << client.second << std::endl;
    }
  }

  std::cout << "End of connected clients (waiting 2s)\n\n";
}

void registerGetConnectedClientsStr() {
  const esp_console_cmd_t cmd = {.command = "getConnectedClientsStr",
                                 .help = "Get connected clients",
                                 .hint = "",
                                 .func = &handled<getConnectedClientsStr>,
                                 .argtable = nullptr};
  ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

void registerCommands() {
  registerGetMode();
  registerSetMode();

  registerGetIp();
  registerWaitForIp();

  registerAddWifiNetwork();
  registerRemoveWifiNetwork();
  registerListWifiNetworks();
  registerSetStaModeOption();
  registerGetStaModeOption();
  registerConnectedSsid();

  registerSetApFallback();
  registerGetApFallback();

  registerSetAp();
  registerGetConnectedClientsStr();
}

extern "C" void app_main() {
  esp_console_repl_t *repl = NULL;
  esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
  repl_config.prompt = "RoFI>";
  esp::EspNvs::initNvs();

  localRoFI = rofi::hal::RoFI::getLocalRoFI();
  if (!localRoFI)
    throw std::runtime_error("No RoFI found");

  wifi =
      std::unique_ptr<esp::EspWifiController>(&esp::EspWifiController::get());
  wifi->initialize();

  registerCommands();

  std::cout
      << "----\nWiFi started with last known configuration - current mode: "
      << wifi->modeStr() << "\n----\n";

  esp_console_dev_uart_config_t uart_config =
      ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));
  ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
