set(PROJECT_NAME unix_example)

set(SRC_FILES
        main.c
        )

set(INC_FOLDERS
        config
        )

if(EXISTS ${HOME}/miniconda/envs/vertices/include AND EXISTS ${HOME}/miniconda/envs/vertices/lib)
    # installed vertices env using miniconda
    include_directories(${HOME}/miniforge3/envs/vertices/include)
    link_directories(${HOME}/miniforge3/envs/vertices/lib)
endif()

if(APPLE)
    # Apple ARM64 specifics:
    # - `miniconda` is better replaced with `miniforge3`
    # - brew-installed packages are located into `/opt/homebrew/`

    if(EXISTS ${HOME}/miniforge3/envs/vertices/include AND EXISTS ${HOME}/miniforge3/envs/vertices/lib)
        # installed vertices env using miniforge3
        include_directories(${HOME}/miniforge3/envs/vertices/include)
        link_directories(${HOME}/miniforge3/envs/vertices/lib)
    elseif(EXISTS /opt/homebrew/include AND EXISTS /opt/homebrew/lib)
        # brew-installed libraries, in case installed in `/opt/homebrew/`
        include_directories(/opt/homebrew/include/)
        link_directories(/opt/homebrew/lib/)
    endif()
endif()

# check that BLOCKCHAIN_PROVIDER variable is set, default to `algorand`
if(DEFINED ENV{BLOCKCHAIN_PROVIDER})
    message(STATUS "Using blockchain: $ENV{BLOCKCHAIN_PROVIDER}")
else()
    set(ENV{BLOCKCHAIN_PROVIDER} algorand)
    message(STATUS "Default blockchain: $ENV{BLOCKCHAIN_PROVIDER}")
endif()

add_definitions(-DCONFIG_PATH="${CMAKE_CURRENT_SOURCE_DIR}/config/")
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

# lots of warnings and all warnings as errors
add_compile_options(-pedantic -Wall -Werror -Wstrict-prototypes -Wwrite-strings -Wshadow -Winit-self -Wcast-align -Wformat=2 -Wmissing-prototypes -Wstrict-overflow=2 -Wc++-compat -Wundef -Wswitch-default -Wconversion -Wno-missing-braces)

# Find cURL
find_package(CURL)
include_directories(${CURL_INCLUDE_DIRS})

# Find Sodium
find_package(Sodium)

# Import Vertices
add_library(Vertices::vertices STATIC IMPORTED)

include_directories(${INC_FOLDERS})
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} sodium curl vertices)
