cmake_minimum_required(VERSION 3.0.0)
set(PROJECT_NAME vertices)
project(${PROJECT_NAME} C ASM)

# Set variable VERTICES_ROOT to absolute path of that file location
get_filename_component(VERTICES_ROOT . ABSOLUTE)

include(utils/utils.cmake)

add_subdirectory(examples/unix)


#set(LWIP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/lwip)
#set(LWIP_INCLUDE_DIRS
#        "${LWIP_DIR}/src/include"
#        "${LWIP_DIR}/contrib"
#        "${LWIP_DIR}/contrib/ports/unix/port/include"
#        "${LWIP_DIR}/contrib/ports/unix/lib"
#        )
#set(LWIP_CONTRIB_DIR ${LWIP_DIR}/contrib)
#set(LWIP_DEFINITIONS LWIP_DEBUG=1)
#
#include(${LWIP_DIR}/src/Filelists.cmake)
#include(${LWIP_DIR}/contrib/Filelists.cmake)


# firmware_size(${PROJECT_NAME})
