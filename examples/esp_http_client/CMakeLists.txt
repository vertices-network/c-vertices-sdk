# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# platformio forces us to have COMPONENT_DIRS different from the original "main"
# thus we add the "src" directory
# if (NOT EXTRA_COMPONENT_DIRS MATCHES "src")
#     list(APPEND EXTRA_COMPONENT_DIRS "src")
# endif()

# This example uses an extra component for common functions such as Wi-Fi and Ethernet connection.
list(APPEND EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)

# Include vertices library
list(APPEND EXTRA_COMPONENT_DIRS ${VERTICES_ROOT}/src)

message(STATUS "extra components dirs ${EXTRA_COMPONENT_DIRS}")


# Import Vertices
# add_library(Vertices::vertices STATIC IMPORTED)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32_app)


idf_build_set_property(COMPILE_OPTIONS "-DBLOCKCHAIN_PROVIDER=$ENV{BLOCKCHAIN_PROVIDER}" APPEND)
