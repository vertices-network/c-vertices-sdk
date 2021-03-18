cmake_minimum_required(VERSION 3.0.0)
set(PROJECT_NAME vertices)
project(${PROJECT_NAME} C ASM)

# Set variable VERTICES_ROOT to absolute path of that file location
get_filename_component(VERTICES_ROOT . ABSOLUTE)

include(utils/utils.cmake)

message(STATUS "💎 Loading targets")
add_subdirectory(lib)
add_subdirectory(examples/unix)
