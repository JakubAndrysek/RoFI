add_rofi_firmware(nimbleDemo.elf
    FLASH_SIZE 4MB
    SOURCES main.c)
target_link_libraries(nimbleDemo.elf idf::freertos idf::esptool_py idf::esp_http_client idf::esp_wifi
idf::lwip idf::console idf::nvs_flash idf::esp_hw_support idf::esp_rom idf::spiffs idf::bt
idf::esp_timer idf::vfs idf::spi_flash idf::esp_eth idf::esp_netif idf::newlib idf::fatfs idf::pthread)
set_partition_table(nimbleDemo.elf partitions.csv)
