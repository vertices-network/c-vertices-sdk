set(PROJECT_NAME unix_example)

set(SRC_FILES
        main.c
        vertices_ports/http_curl.c
        )

set(INC_FOLDERS
        config
        )

# check that BLOCKCHAIN_PROVIDER variable is set, default to `algorand`
if(DEFINED ENV{BLOCKCHAIN_PROVIDER})
    message(STATUS "Using blockchain: $ENV{BLOCKCHAIN_PROVIDER}")
else()
    set(ENV{BLOCKCHAIN_PROVIDER} algorand)
    message(STATUS "Default blockchain: $ENV{BLOCKCHAIN_PROVIDER}")
endif()

import_libs()

add_definitions(-DCONFIG_PATH="${CMAKE_CURRENT_SOURCE_DIR}/config/")
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

if ($ENV{DOCKER_RUN})
    # From Docker (CI jobs) we want to send only 100 micro algos to Bob
    # so that we don't have to put money into Alice's account too often
    add_definitions("-DAMOUNT_SENT=100")
else()
    # Generic example sends 1 algo
    add_definitions("-DAMOUNT_SENT=1000000")
endif()

# lots of warnings and all warnings as errors
add_compile_options(-pedantic -Wall -Werror -Wstrict-prototypes -Wwrite-strings -Wshadow -Winit-self -Wcast-align -Wformat=2 -Wmissing-prototypes -Wstrict-overflow=2 -Wc++-compat -Wundef -Wswitch-default -Wconversion -Wno-missing-braces)

# Find cURL
find_package(CURL)
include_directories(${CURL_INCLUDE_DIRS})

# Import Vertices
add_library(Vertices::vertices STATIC IMPORTED)

include_directories(${INC_FOLDERS})
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} sodium curl vertices)
