cmake_minimum_required(VERSION 3.6)

add_rofi_firmware(esp_nvs.elf
    FLASH_SIZE 4MB
    SOURCES main.cpp)
target_link_libraries(esp_nvs.elf
    idf::freertos idf::nvs_flash rofi::hal::esp32_jig rofi::esp32)
set_partition_table(esp_nvs.elf partitions.csv)
