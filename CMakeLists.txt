cmake_minimum_required(VERSION 3.0.0)
set(PROJECT_NAME Vertices-SDK)
project(${PROJECT_NAME})

# Set variable VERTICES_ROOT to absolute path of that file location
get_filename_component(VERTICES_ROOT . ABSOLUTE)

include(utils/utils.cmake)

message(STATUS "💎 Loading targets")
add_subdirectory(lib)
add_subdirectory(examples/unix)
