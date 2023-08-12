#include "BLEConnector.h"
#include "iostream"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" {void app_main(void);}

void app_main(void) {
  // std::cout << "Hello world!" << std::endl;
  BLEConnector connector("RoFI");
  connector.start();

  connector.setValue("Ahoj");

  int counter = 0;
  while (true)
  {
    // connector.setValue("Count" + std::to_string(counter));
    connector.setId(counter);
    connector.updateJoint(0);
    // connector.updateConnector(0);
    connector.sendNewValue();
    counter++;
    std::cout << "Count: " << std::to_string(counter) << std::endl;
    vTaskDelay(1000 / portTICK_PERIOD_MS);

  }
}
