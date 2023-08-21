add_rofi_firmware(nimbleCppNew.elf
    FLASH_SIZE 4MB
    SOURCES main.cpp)
target_link_libraries(nimbleCppNew.elf
    idf::freertos idf::spi_flash idf::console rofi::hal rofi::hal::js jac-device idf::vfs idf::fatfs idf::pthread esp_nimble_cpp)
set_partition_table(nimbleCppNew.elf partitions-4m.csv)

# target_compile_definitions(nimbleCppNew.elf PRIVATE "ESP_PLATFORM=\"1\"")
# target_compile_definitions(nimbleCppNew.elf PRIVATE "CONFIG_IDF_TARGET_ESP32=\"1\"")


message(STATUS "Creating spiflash image for nimbleCppNew.elf")
