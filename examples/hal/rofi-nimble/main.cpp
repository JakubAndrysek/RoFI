#include <nimBleRofi.hpp>
#include <rofi_hal.hpp>

extern "C" {
void app_main(void);
}

void app_main(void) {
  printf("Starting RoFI NimBLE Server\n");
  rofi::hal::RoFI localRoFI = rofi::hal::RoFI::getLocalRoFI();

  // NimBleRofi
  rofi::nimble::NimBleRofi nimBleRofi = rofi::nimble::NimBleRofi(localRoFI);
  nimBleRofi.start();

  // Do wathever you want here - the server runs in the background
  while (true) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
