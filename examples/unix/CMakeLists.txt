set(PROJECT_NAME unix_example)

set(SRC_FILES
        main.c
        )

set(INC_FOLDERS
        config
        )

# brew-installed libraries, in case installed in `/opt/homebrew/`
if(APPLE)
if(EXISTS /opt/homebrew/include AND EXISTS /opt/homebrew/lib)
include_directories(/opt/homebrew/include/)
link_directories(/opt/homebrew/lib/)
endif()
elseif(UNIX)
message("You might need to add path to libsodium (headers and library)")
endif()

set(CFLAGS -DALGORAND_PROVIDER=1 )
add_definitions(-DCONFIG_PATH="${CMAKE_CURRENT_SOURCE_DIR}/config/")

find_package(CURL)
include_directories(${CURL_INCLUDE_DIRS})

add_library(Vertices::vertices STATIC IMPORTED)

include_directories(${INC_FOLDERS})
set(CMAKE_C_FLAGS ${CFLAGS})

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} sodium curl vertices)
