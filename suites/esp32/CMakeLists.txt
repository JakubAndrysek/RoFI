cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{ROFI_ROOT}/releng/cmake)
include(FetchContent)
include(CollectTargets)
include(ESP32)
include(ColoredOutput)

project(rofi)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts")

# Specify common dependencies
FetchContent_Declare(
  magic_enum
  GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
  GIT_TAG        v0.7.3
)
FetchContent_GetProperties(magic_enum)
if(NOT magic_enum_POPULATED)
  FetchContent_Populate(magic_enum)
  add_subdirectory(${magic_enum_SOURCE_DIR} ${magic_enum_BINARY_DIR})
endif()

FetchContent_Declare(
  lib_fort
  GIT_REPOSITORY https://github.com/seleznevae/libfort.git
  GIT_TAG        v0.4.2
)
FetchContent_GetProperties(lib_fort)
if(NOT lib_fort_POPULATED)
  FetchContent_Populate(lib_fort)
  option(FORT_ENABLE_TESTING "Enables building tests and examples" OFF)
  add_subdirectory(${lib_fort_SOURCE_DIR} ${lib_fort_BINARY_DIR})
endif()


function(add_rofi_firmware name)
    cmake_parse_arguments(A "" "FLASH_SIZE" "SOURCES" ${ARGN})
    add_executable(${name} ${A_SOURCES})
    idf_build_executable(${name})
    generate_image_from(${name} "${A_FLASH_SIZE}")
endfunction()

function(add_component name)
    add_subdirectory($ENV{ROFI_ROOT}/softwareComponents/${name} ${CMAKE_CURRENT_BINARY_DIR}/${name})
endfunction()

function(add_application path)
    add_subdirectory($ENV{ROFI_ROOT}/${path} ${CMAKE_CURRENT_BINARY_DIR}/${path})
endfunction()

idf_build_process(esp32
    COMPONENTS idf::freertos idf::esptool_py idf::esp_http_client idf::esp_wifi
               idf::lwip idf::console idf::nvs_flash idf::esp_hw_support idf::esp_rom
               idf::esp_timer idf::bt
    SDKCONFIG ${CMAKE_CURRENT_LIST_DIR}/sdkconfig
    BUILD_DIR ${CMAKE_BINARY_DIR})

# What follows is a huge, huge hack. Unless we link idf::freertos from the
# top-level CMakeLists.txt, we get generator error about self-refereincing
# sources. Therefore, let's create a dummy binary and link it.
set(HACK_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/hack_src.c)
file(GENERATE OUTPUT ${HACK_SOURCE}
    CONTENT "void app_main() {}\n")
add_executable(build_hack ${HACK_SOURCE})
idf_build_executable(build_hack)
target_link_libraries(build_hack idf::freertos)
set_target_properties(build_hack PROPERTIES EXCLUDE_FROM_ALL "exclude-NOTFOUND")

# Add component https://github.com/h2zero/esp-nimble-cpp
FetchContent_Declare(
  esp_nimble_cpp
  GIT_REPOSITORY https://github.com/h2zero/esp-nimble-cpp.git
  GIT_TAG        21e1217e4ce520b276695dfc60e6151750561617 #v1.4.1
)
FetchContent_GetProperties(esp_nimble_cpp)
if(NOT esp_nimble_cpp_POPULATED)
  FetchContent_Populate(esp_nimble_cpp)

  # add_subdirectory(${esp_nimble_cpp_SOURCE_DIR} ${esp_nimble_cpp_BINARY_DIR})
  file(GLOB_RECURSE SRC ${esp_nimble_cpp_SOURCE_DIR}/src/*.cpp ${esp_nimble_cpp_SOURCE_DIR}/src/*.c ${esp_nimble_cpp_SOURCE_DIR}/src/*.h)

  add_library(esp-nimble-cpp ${SRC})
  include_directories(${esp_nimble_cpp_SOURCE_DIR}/src)
  target_include_directories(esp-nimble-cpp PUBLIC src)
  set_property(TARGET esp-nimble-cpp PROPERTY CXX_STANDARD 20)

  target_compile_definitions(esp-nimble-cpp PUBLIC ESP_PLATFORM CONFIG_NIMBLE_CPP_ATT_VALUE_TIMESTAMP_ENABLED)
  target_link_libraries(esp-nimble-cpp PUBLIC idf::freertos idf::nvs_flash idf::spi_flash idf::bt rofi::hal)
endif()


# Add component https://github.com/nanopb/nanopb
FetchContent_Declare(
  nanopb
  GIT_REPOSITORY https://github.com/nanopb/nanopb.git
  GIT_TAG        nanopb-0.4.8
)

FetchContent_GetProperties(nanopb)
if(NOT nanopb_POPULATED)
  FetchContent_Populate(nanopb)

  # add "./pb_common.c" "./pb_decode.c" "./pb_encode.c"
  file(GLOB_RECURSE SRC ${nanopb_SOURCE_DIR}/pb_common.c ${nanopb_SOURCE_DIR}/pb_decode.c ${nanopb_SOURCE_DIR}/pb_encode.c)

  add_library(nanopb ${SRC})
  include_directories(${nanopb_SOURCE_DIR})
  target_include_directories(nanopb PUBLIC ${nanopb_SOURCE_DIR})
  set_property(TARGET nanopb PROPERTY CXX_STANDARD 20)
endif()


add_component(atoms)
add_component(lwip++)
add_component(lwipcore)
add_component(rofiHalInc)
add_component(rofiHalEsp32)
add_component(libfi)
add_component(firmwareUpdater)
add_component(nimBleRofi)
add_component(nimBleConnector)
# Introduce alias
add_library(rofi::hal ALIAS rofi_hal_esp32)

# Add utilities
add_application(hardwareModules/universalModule/software/esp32/dynamixelConfigurator)
add_application(hardwareModules/universalModule/software/esp32/console)


# Add the examples
add_application(hardwareModules/universalModule/software/esp32/esp_hello_world)
add_application(examples/hal/wifi)
add_application(examples/hal/servo)
add_application(examples/hal/connectorHal)
add_application(examples/hal/connectorMovementHal)
add_application(examples/hal/connectorHalStatus)
add_application(examples/hal/firmwareUpdater)
add_application(examples/hal/rofi-nimble)
add_application(examples/hal/rofi-nimble-basic)
add_application(examples/hal/rofi-nimble-protobf)



set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
list_targets()
