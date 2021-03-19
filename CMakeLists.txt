cmake_minimum_required(VERSION 3.0.0)
set(PROJECT_NAME Vertices-SDK)
project(${PROJECT_NAME})

# Set variable VERTICES_ROOT to absolute path of that file location
get_filename_component(VERTICES_ROOT . ABSOLUTE)

include(utils/utils.cmake)

set(CMAKE_MACOSX_RPATH 1)

message(STATUS "💎 Loading targets")
add_subdirectory(external/cJSON)
add_subdirectory(lib)

# examples
add_subdirectory(examples/unix)
