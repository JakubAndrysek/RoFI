cmake_minimum_required(VERSION 3.6)

add_rofi_firmware(set_id.elf
    FLASH_SIZE 4MB
    SOURCES main.cpp)
target_link_libraries(set_id.elf
    idf::freertos idf::nvs_flash rofi::hal::esp32_jig rofi::esp32 idf::console libfort::fort)
set_partition_table(set_id.elf partitions.csv)
