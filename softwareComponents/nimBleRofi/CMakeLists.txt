# Include the generated files directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/protoc)

# Find all source files, including those in src/protoc
file(GLOB_RECURSE SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/protoc/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/protoc/*.c
)

# Macro to setup the HAL library
macro(setup_hal NAME)
    add_library("${NAME}" ${SRC})
    target_include_directories("${NAME}" PUBLIC src)
    set_property(TARGET "${NAME}" PROPERTY CXX_STANDARD 20)
    target_link_libraries("${NAME}" PUBLIC idf::freertos idf::spi_flash idf::esp_phy rofi::hal idf::bt idf::esp_timer esp-nimble-cpp nanopb)
    target_compile_options("${NAME}" PUBLIC)
endmacro()

# Setup the rofi_nimble library
setup_hal(rofi_nimble)
add_library(rofi::nimble ALIAS rofi_nimble)

# # Define the custom target to generate protocol files
# add_custom_target(generate-rofi-protocol ALL
#     COMMAND ${CMAKE_COMMAND} -E env make generate-protocol
#     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#     COMMENT "Generating protocol files"
# )

# # Ensure the protocol files are generated before compiling sources
# add_dependencies(rofi_nimble generate-rofi-protocol)