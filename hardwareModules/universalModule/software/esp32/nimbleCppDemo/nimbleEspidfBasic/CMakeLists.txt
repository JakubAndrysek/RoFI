add_rofi_firmware(nimbleDemoBasic.elf
    FLASH_SIZE 4MB
    SOURCES main.cpp)
target_link_libraries(nimbleDemoBasic.elf esp_nimble_cpp idf::freertos idf::esptool_py idf::esp_http_client idf::esp_wifi
idf::lwip idf::console idf::nvs_flash idf::esp_hw_support idf::esp_rom idf::spiffs idf::bt
idf::esp_timer idf::vfs idf::spi_flash idf::esp_eth idf::esp_netif idf::newlib idf::fatfs idf::pthread)


# ESP_PLATFORM
set(ESP_PLATFORM esp32)

# set_partition_table(nimbleDemoBasic.elf partitions.csv)
