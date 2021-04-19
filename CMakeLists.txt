cmake_minimum_required(VERSION 3.0.0)
set(PROJECT_NAME Vertices-SDK)
project(${PROJECT_NAME})

# Set variable VERTICES_ROOT to absolute path of that file location
get_filename_component(VERTICES_ROOT . ABSOLUTE)
add_definitions(-DVERTICES_ROOT="${VERTICES_ROOT}")

include(utils/utils.cmake)

set(CMAKE_MACOSX_RPATH 1)

message(STATUS "💎 Loading targets")

# Module path to find libs
set(CMAKE_MODULE_PATH ${VERTICES_ROOT}/utils/)

# Set blockchain
set(ENV{BLOCKCHAIN_PROVIDER} algorand)
add_subdirectory(examples/unix)
add_subdirectory(external/cJSON)
add_subdirectory(external/mbedtls)
add_subdirectory(lib)
