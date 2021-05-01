message(STATUS "Vertices SDK location: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Vertices SDK provider: $ENV{BLOCKCHAIN_PROVIDER}")

set(VERTICES_SRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/account.c
        ${CMAKE_CURRENT_SOURCE_DIR}/base64.c
        ${CMAKE_CURRENT_SOURCE_DIR}/base32.c
        ${CMAKE_CURRENT_SOURCE_DIR}/sha512_256.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vertices_errors.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vertices.c
        ${CMAKE_CURRENT_SOURCE_DIR}/$ENV{BLOCKCHAIN_PROVIDER}/$ENV{BLOCKCHAIN_PROVIDER}_provider.c
        ${CMAKE_CURRENT_SOURCE_DIR}/$ENV{BLOCKCHAIN_PROVIDER}/$ENV{BLOCKCHAIN_PROVIDER}_transaction.c
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
    list(APPEND VERTICES_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/http/http_esp.c)

    list(APPEND compile_options "-DBLOCKCHAIN_PROVIDER=$ENV{BLOCKCHAIN_PROVIDER}")
    list(APPEND compile_options "-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

    idf_component_register(SRCS ${VERTICES_SRC_FILES}
            INCLUDE_DIRS ../include include
            PRIV_INCLUDE_DIRS "${VERTICES_INC_DIRS}"
            REQUIRES mbedtls esp_http_client
            )

elseif(ZEPHYR_BASE)

    # Zephyr implementation is on the roadmap

else ()
    # Add platform-specific http wrapper provided by Vertices
    list(APPEND VERTICES_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/http/http_curl.c)

    add_definitions(-DBLOCKCHAIN_PROVIDER=$ENV{BLOCKCHAIN_PROVIDER})
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
    install(FILES ../include/vertices.h ../include/vertices_errors.h ../include/vertices_types.h DESTINATION include)

    install(TARGETS vertices
            EXPORT verticesTargets
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            RUNTIME DESTINATION bin
            INCLUDES DESTINATION include
            )

    target_link_libraries(vertices cjson mbedcrypto)
    target_compile_options(vertices PRIVATE -pedantic -Wall -Werror -Wstrict-prototypes -Wwrite-strings -Wshadow -Winit-self -Wformat=2 -Wmissing-prototypes -Wstrict-overflow=2 -Wc++-compat -Wundef -Wconversion -Wno-missing-braces)

endif ()