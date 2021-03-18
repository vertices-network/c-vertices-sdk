set(PROJECT_NAME unix_example)

set(SRC_FILES
        main.c
        )

set(INC_FOLDERS
        )

set(CFLAGS)

find_package(CURL)
include_directories(${CURL_INCLUDE_DIRS})

add_library(Vertices::vertices STATIC IMPORTED)

include_directories(${INC_FOLDERS})
set(CMAKE_C_FLAGS ${CFLAGS})

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} curl vertices)
