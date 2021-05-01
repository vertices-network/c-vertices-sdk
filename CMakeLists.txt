cmake_minimum_required(VERSION 3.5)
set(PROJECT_NAME Vertices-SDK)
project(${PROJECT_NAME})

# Set variable VERTICES_ROOT to absolute path of that file location
get_filename_component(VERTICES_ROOT . ABSOLUTE)
add_definitions(-DVERTICES_ROOT="${VERTICES_ROOT}")

include(utils/utils.cmake)

message(STATUS "💎 Loading targets")

# Module path to find libs
set(CMAKE_MODULE_PATH ${VERTICES_ROOT}/utils/)

# Set blockchain
set(ENV{BLOCKCHAIN_PROVIDER} algorand)

# Add vertices library
add_subdirectory(src)

# At this point there are two different targets possible:
# - ESP32 microcontrollers
# - Unix-based OS
# Check whether running from idf.py or if CMAKE_TOOLCHAIN_FILE is
# set to utils/toolchain-esp32.cmake
if (COMPILE_ESP OR ESP_PLATFORM)
    # set project name for generated ESP32 elf/binary files
    project(esp32_example)

    # add_subdirectory(examples/esp32)
    add_subdirectory(examples/esp_http_client)
else ()
    set(CMAKE_MACOSX_RPATH 1)

    add_subdirectory(examples/unix)
    add_subdirectory(external/mbedtls)
    add_subdirectory(external/cJSON)
endif ()


