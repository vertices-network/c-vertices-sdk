set(PROJECT_NAME unix_example)

set(SRC_FILES
        main.c
        )

set(INC_FOLDERS
        ${VERTICES_INC_FOLDERS}
        )

set(CFLAGS)

message(STATUS "Looking for cURL library")
find_package(CURL)
include_directories(${CURL_INCLUDE_DIRS})

message(STATUS "Looking for Vertices SDK")
add_library(Vertices::vertices STATIC IMPORTED)

include_directories(${INC_FOLDERS})
set(CMAKE_C_FLAGS ${CFLAGS})

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} curl vertices)
