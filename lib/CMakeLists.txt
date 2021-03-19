set(VERTICES_SRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/vertices_errors.c
        ${CMAKE_CURRENT_SOURCE_DIR}/vertices.c
        ${CMAKE_CURRENT_SOURCE_DIR}/account.c
        ${CMAKE_CURRENT_SOURCE_DIR}/http/http_curl.c
        ${CMAKE_CURRENT_SOURCE_DIR}/providers/algorand_provider.c
        )

message(STATUS "Vertices SDK: ${CMAKE_CURRENT_SOURCE_DIR}")

add_library(vertices STATIC ${VERTICES_SRC_FILES})
target_include_directories(vertices
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../inc>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../mdk>"
        "$<INSTALL_INTERFACE:../inc>"
        )

# public header files
install(FILES ../inc/vertices.h ../inc/vertices_errors.h ../inc/vertices_types.h DESTINATION inc)

install(TARGETS vertices
        EXPORT verticesTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION inc
        )

install(EXPORT verticesTargets
        FILE verticesTargets.cmake
        NAMESPACE Vertices::
        DESTINATION lib/cmake/vertices
        )
