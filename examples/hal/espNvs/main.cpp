#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <iostream>
#include <limits>
#include <nvs/nvs.hpp>
#include <rofi_hal.hpp>
#include <sstream>

void rawNvs() {
  auto nvs = esp::EspNvs::open("rawNvs");
  nvs->setInt("test", 42);
  std::cout << "Value: " << nvs->getInt("test") << std::endl;

  // Test multiple values
  for (int i = 0; i < 10; i++) {
    std::stringstream ss;
    ss << "test" << i;
    nvs->setInt(ss.str(), i);
  }

  for (int i = 0; i < 10; i++) {
    std::stringstream ss;
    ss << "test" << i;
    std::cout << "Value: " << nvs->getInt(ss.str()) << std::endl;
  }
}

extern "C" void app_main() {
  std::cout << "Program starts\n";

  std::cout << "Init NVS\n";
  esp::EspNvs::initNvs();

  std::cout << "Working with NVS\n";
  rawNvs();

  auto rofi = rofi::hal::RoFI::getLocalRoFI();

  std::cout << "Rofi read ID\n";
  std::cout << "Rofi ID: " << rofi.getId() << std::endl;
}
