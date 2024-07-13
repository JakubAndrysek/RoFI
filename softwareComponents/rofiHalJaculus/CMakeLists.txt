add_library(rofiHalJaculus INTERFACE)
target_include_directories(rofiHalJaculus INTERFACE .)

target_link_libraries(rofiHalJaculus INTERFACE jac-dcore jac-machine jac-link
	quickjs idf::nvs_flash idf::vfs idf::driver idf::pthread idf::spiffs
	idf::fatfs idf::esp_timer
)

# Esp32-RBGridUI

add_library(rofi::hal::js ALIAS rofiHalJaculus)

# ESP_PLATFORM enables quickjs compilation flags for building on ESP32 platforms
target_compile_definitions(quickjs PUBLIC -DESP_PLATFORM)