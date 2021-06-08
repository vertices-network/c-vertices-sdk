
function(import_libs)

if(EXISTS $ENV{CONDA_PREFIX}/include AND EXISTS $ENV{CONDA_PREFIX}/lib)
    # installed vertices environment with conda
    if($ENV{CONDA_PREFIX} MATCHES "vertices")
        message(STATUS "Importing libs from Conda env (vertices): $ENV{CONDA_PREFIX}")
        include_directories($ENV{CONDA_PREFIX}/include)
        link_directories($ENV{CONDA_PREFIX}/lib)
    endif()
endif()

if(APPLE)
    # Apple ARM64 specifics:
    # - brew-installed packages are located into `/opt/homebrew/`
    if(EXISTS /opt/homebrew/include AND EXISTS /opt/homebrew/lib)
        # brew-installed libraries, in case installed in `/opt/homebrew/`
        include_directories(/opt/homebrew/include/)
        link_directories(/opt/homebrew/lib/)
    endif()
endif()

endfunction()