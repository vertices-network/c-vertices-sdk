# SPDX-License-Identifier: Apache-2.0
message(STATUS "Vertices SDK location: ${CMAKE_CURRENT_SOURCE_DIR}")

if ($ENV{BLOCKCHAIN_PROVIDER})
    set(BLOCKCHAIN_PROVIDER $ENV{BLOCKCHAIN_PROVIDER})
elseif(NOT BLOCKCHAIN_PROVIDER)
    message(STATUS "Defaulting to Algorand")
    set(BLOCKCHAIN_PROVIDER algorand)
endif()

message(STATUS "Vertices SDK provider: ${BLOCKCHAIN_PROVIDER}")

set(VERTICES_SRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/account.c
        ${CMAKE_CURRENT_SOURCE_DIR}/base64.c
        ${CMAKE_CURRENT_SOURCE_DIR}/base32.c
        ${CMAKE_CURRENT_SOURCE_DIR}/http_weak.c
        ${CMAKE_CURRENT_SOURCE_DIR}/sha512_256.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vertices_errors.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vertices.c
        ${CMAKE_CURRENT_SOURCE_DIR}/${BLOCKCHAIN_PROVIDER}/${BLOCKCHAIN_PROVIDER}_parser.c
        ${CMAKE_CURRENT_SOURCE_DIR}/${BLOCKCHAIN_PROVIDER}/${BLOCKCHAIN_PROVIDER}_provider.c
        ${CMAKE_CURRENT_SOURCE_DIR}/${BLOCKCHAIN_PROVIDER}/${BLOCKCHAIN_PROVIDER}_transaction.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/mpack/src/mpack/mpack-common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/mpack/src/mpack/mpack-reader.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/mpack/src/mpack/mpack-platform.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/mpack/src/mpack/mpack-expect.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/mpack/src/mpack/mpack-node.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/mpack/src/mpack/mpack-writer.c
        )

set(VERTICES_INC_DIRS
        "include"
        "../external/cJSON"
        "../external/mpack/src/mpack"
        )

string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)

if (ESP_PLATFORM)
    # Add platform-specific http wrapper provided by Vertices

    list(APPEND compile_options "-DBLOCKCHAIN_PROVIDER=${BLOCKCHAIN_PROVIDER}")
    list(APPEND compile_options "-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

    idf_component_register(SRCS ${VERTICES_SRC_FILES}
            INCLUDE_DIRS ../include include
            PRIV_INCLUDE_DIRS "${VERTICES_INC_DIRS}"
            REQUIRES mbedtls esp_http_client
            )

elseif (COMPILE_ZEPHYR)

    list(APPEND compile_options "-DBLOCKCHAIN_PROVIDER=${BLOCKCHAIN_PROVIDER}")
    list(APPEND compile_options "-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

    zephyr_interface_library_named(vertices)
    zephyr_library()
    zephyr_library_sources(${VERTICES_SRC_FILES})
    zephyr_include_directories(
            ${VERTICES_INC_DIRS}
            ../include
    )

    zephyr_compile_options("-DBLOCKCHAIN_PROVIDER=${BLOCKCHAIN_PROVIDER}")
    zephyr_compile_options("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

    zephyr_compile_options(-DMPACK_STDIO=0)
    zephyr_compile_options(-DMPACK_STDLIB=0)

    zephyr_library_link_libraries(vertices)
    target_link_libraries(vertices INTERFACE zephyr_interface)

    message(STATUS "🪁 Added Vertices library to Zephyr")

elseif (COMPILE_UNIX)

    add_definitions(-DBLOCKCHAIN_PROVIDER=${BLOCKCHAIN_PROVIDER})
    add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

    add_library(vertices STATIC ${VERTICES_SRC_FILES})

    target_include_directories(vertices
            PUBLIC
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>"
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../external/cJSON>"
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../external/mbedtls/library>"
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../external/ed25519>"
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../external/mpack/src/mpack>"
            "$<INSTALL_INTERFACE:../include>"
            )

    # public header files
    install(FILES ../include/vertices.h ../include/vertices_errors.h ../include/vertices_types.h ../include/vertices_http.h DESTINATION include)

    install(TARGETS vertices
            EXPORT verticesTargets
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            RUNTIME DESTINATION bin
            INCLUDES DESTINATION include
            )

    target_link_libraries(vertices cjson mbedcrypto)
    target_compile_options(vertices PRIVATE -pedantic -Wall -Werror -Wstrict-prototypes -Wwrite-strings -Wshadow -Winit-self -Wformat=2 -Wmissing-prototypes -Wstrict-overflow=2 -Wc++-compat -Wundef -Wconversion -Wno-missing-braces)

else ()
    target_compile_options(mbedcrypto PRIVATE -DMBEDTLS_CONFIG_FILE=<${CMAKE_CURRENT_SOURCE_DIR}/include/mbedtls_config.h>)

    add_definitions(-DBLOCKCHAIN_PROVIDER=${BLOCKCHAIN_PROVIDER})

    add_library(vertices STATIC ${VERTICES_SRC_FILES})
    target_include_directories(vertices PUBLIC ../include PRIVATE ${VERTICES_INC_DIRS})

    target_link_libraries(vertices cjson mbedcrypto)
endif ()