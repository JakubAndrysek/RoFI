cmake_minimum_required(VERSION 3.6)

add_rofi_firmware(wifi_controller_console.elf
    FLASH_SIZE 4MB
    SOURCES main.cpp)
target_link_libraries(wifi_controller_console.elf
    idf::freertos idf::nvs_flash rofi::hal::esp32_jig rofi::esp32 idf::console libfort::fort)
set_partition_table(wifi_controller_console.elf partitions.csv)
