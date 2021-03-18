cmake_minimum_required(VERSION 3.0.0)
set(PROJECT_NAME vertices)
project(${PROJECT_NAME} C ASM)

include(utils/utils.cmake)

set(SRC_FILES
        src/vertices_errors.c
        src/vertices.c
        src/account.c
        src/tcp/tcp_unix.c
        )

set(INC_FOLDERS
        inc
        mdk
        config)

set(CFLAGS)

set(LWIP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/lwip)
set(LWIP_INCLUDE_DIRS
        "${LWIP_DIR}/src/include"
        "${LWIP_DIR}/contrib"
        "${LWIP_DIR}/contrib/ports/unix/port/include"
        "${LWIP_DIR}/contrib/ports/unix/lib"
        )
set(LWIP_CONTRIB_DIR ${LWIP_DIR}/contrib)
set(LWIP_DEFINITIONS LWIP_DEBUG=1)

include(${LWIP_DIR}/src/Filelists.cmake)
include(${LWIP_DIR}/contrib/Filelists.cmake)

include_directories(${INC_FOLDERS})
set(CMAKE_C_FLAGS ${CFLAGS})

add_executable(vertices examples/unix/main.c ${SRC_FILES})
target_include_directories(vertices PRIVATE ${LWIP_INCLUDE_DIRS})
target_link_libraries(vertices lwipcontribapps lwipallapps lwipcore)


# firmware_size(${PROJECT_NAME})
