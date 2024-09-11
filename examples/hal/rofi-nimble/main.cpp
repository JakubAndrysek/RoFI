#include <nimBleRofi.hpp>
#include <rofi_hal.hpp>

extern "C" {
void app_main(void);
}

void app_main(void) {
  rofi::hal::RoFI localRoFI = rofi::hal::RoFI::getLocalRoFI();
  printf("Starting RoFI NimBLE Server - RoFI ID: %d\n", localRoFI.getId());

  // NimBleRofi
  rofi::nimble::NimBleRofi nimBleRofi = rofi::nimble::NimBleRofi(localRoFI);
  nimBleRofi.start();

  // Do wathever you want here - the server runs in the background
  while (true) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
