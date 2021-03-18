set(PROJECT_NAME unix_example)

include(${VERTICES_ROOT}/src/vertices.cmake)

set(SRC_FILES
        main.c
        ${VERTICES_SRC_FILES}
        )

set(INC_FOLDERS
        ${VERTICES_INC_FOLDERS}
        )

set(CFLAGS)

find_package( CURL )
include_directories( ${CURL_INCLUDE_DIRS} )

include_directories(${INC_FOLDERS})
set(CMAKE_C_FLAGS ${CFLAGS})

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} curl)
