# Welcome to the ESP32 example!
# ## ESP_IDF
# With ESP-IDF installed somewhere on your computer,
# you need to make sure you have sourced all the variables (`. $IDF_PATH/export.sh`)
# ## PlatformIO
# The example can be imported directly in PlatformIO
#
# Have fun!

cmake_minimum_required(VERSION 3.5)

# Include vertices library from local directory if we are not building from PlatformIO
string(FIND $ENV{IDF_PATH} ".platformio" is_platformio)
if (${is_platformio} LESS 0)
    if (DEFINED VERTICES_ROOT)
        list(APPEND EXTRA_COMPONENT_DIRS
                ${VERTICES_ROOT}/src
                )
    else ()
        # consider that we are building from current file's directory in the Vertices SDK
        list(APPEND EXTRA_COMPONENT_DIRS
                ${CMAKE_CURRENT_LIST_DIR}/../../src
                )
    endif ()
endif ()

if (NOT DEFINED ENV{BLOCKCHAIN_PROVIDER})
    set(ENV{BLOCKCHAIN_PROVIDER} algorand)
    message(STATUS "Using default Blockchain provider: $ENV{BLOCKCHAIN_PROVIDER}")
endif ()

# Init ESP-IDF build system
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# This example uses an extra component for common functions such as Wi-Fi and Ethernet connection.
list(APPEND EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)

project(esp32_app)

# Make sure we are compiling for the BLOCKCHAIN_PROVIDER
idf_build_set_property(COMPILE_OPTIONS "-DBLOCKCHAIN_PROVIDER=$ENV{BLOCKCHAIN_PROVIDER}" APPEND)
