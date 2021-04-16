# Get all subdirectories under ${current_dir} and store them
# in ${result} variable
macro(subdirlist result current_dir)
    file(GLOB children ${current_dir}/*)
    set(dirlist "")

    foreach(child ${children})
        if (IS_DIRECTORY ${child})
            list(APPEND dirlist ${child})
        endif()
    endforeach()

    set(${result} ${dirlist})
endmacro()

# Prepend ${CMAKE_CURRENT_SOURCE_DIR} to a ${directory} name
# and save it in PARENT_SCOPE ${variable}
macro(prepend_cur_dir variable directory)
    set(${variable} ${CMAKE_CURRENT_SOURCE_DIR}/${directory})
endmacro()

# Add custom command to print firmware size in Berkley format
function(firmware_size target)
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_SIZE_UTIL} -B
            "${CMAKE_CURRENT_BINARY_DIR}/${target}${CMAKE_EXECUTABLE_SUFFIX}"
            )
endfunction()

# Add a command to generate firmare in a provided format
function(generate_object target suffix type)
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -O${type}
            "${CMAKE_CURRENT_BINARY_DIR}/${target}${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_BINARY_DIR}/${target}${suffix}"
            )
endfunction()


# In case running on arm64 arch
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")
        # Check whether we are running under Rosetta on arm64 hardware.
        execute_process(COMMAND sysctl -q hw.optional.arm64
                OUTPUT_VARIABLE _sysctl_stdout
                ERROR_VARIABLE _sysctl_stderr
                RESULT_VARIABLE _sysctl_result
                )
        if(_sysctl_result EQUAL 0 AND _sysctl_stdout MATCHES "hw.optional.arm64: 1")
            set(CMAKE_HOST_SYSTEM_PROCESSOR "arm64")
        endif()
    endif()
    SET(CMAKE_OSX_ARCHITECTURES "${CMAKE_HOST_SYSTEM_PROCESSOR}" CACHE STRING "Build architectures for Mac OS X" FORCE)
endif()