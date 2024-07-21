add_rofi_firmware(umRofiNimble.elf
    FLASH_SIZE 4MB
    SOURCES main.cpp)
target_link_libraries(umRofiNimble.elf
    idf::freertos idf::spi_flash idf::esp_phy rofi::hal idf::bt idf::esp_timer esp-nimble-cpp nanopb rofi::nimble)
set_partition_table(umRofiNimble.elf partitions.csv)
