# Pulse Engine - Utility CMake Functions
# This file contains low-level utility functions used throughout the build system

# Function to print colored messages for better readability in the terminal
# Usage: pulse_message(STATUS "Building module X")
function(pulse_message TYPE MESSAGE)
    # Add a prefix to make pulse-specific messages easily identifiable
    message(${TYPE} "[Pulse] ${MESSAGE}")
endfunction()

# Function to collect all subdirectories in a given path
# This is useful for automatically discovering modules
# Usage: pulse_collect_subdirectories(RESULT_VAR "path/to/scan")
function(pulse_collect_subdirectories OUT_VAR SEARCH_PATH)
    # Get all items in the directory
    file(GLOB CHILDREN RELATIVE ${SEARCH_PATH} ${SEARCH_PATH}/*)

    set(DIR_LIST "")
    foreach(CHILD ${CHILDREN})
        # Check if this item is a directory and contains a CMakeLists.txt
        set(FULL_PATH ${SEARCH_PATH}/${CHILD})
        if(IS_DIRECTORY ${FULL_PATH})
            if(EXISTS ${FULL_PATH}/CMakeLists.txt)
                list(APPEND DIR_LIST ${CHILD})
            endif()
        endif()
    endforeach()

    # Return the list to the caller
    set(${OUT_VAR} ${DIR_LIST} PARENT_SCOPE)
endfunction()

# Function to set common compiler flags and options for all targets
# This ensures consistency across your entire project
function(pulse_set_common_properties TARGET_NAME)
    # Set C++ standard to C++17 (adjust as needed for your engine)
    ##target_compile_features(${TARGET_NAME} PUBLIC cxx_std_20)

    # todo : attempt for p2996 compiler
    target_compile_options(${TARGET_NAME} PUBLIC -std=c++2c -freflection-latest -stdlib=libc++)
    target_link_options(${TARGET_NAME} PUBLIC -stdlib=libc++)

    # Enable position-independent code for better library compatibility
    set_target_properties(${TARGET_NAME} PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        CXX_VISIBILITY_PRESET hidden        # Hide symbols by default
        VISIBILITY_INLINES_HIDDEN ON        # Hide inline functions
        CXX_EXTENSIONS OFF                  # Disable compiler extensions
    )

    # Add common compiler warnings to catch potential issues early
    if(MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE
            /W4
            /w14640  # Thread-safe static initialization
            /w14265  # Virtual destructor warnings
        )
    else()
        target_compile_options(${TARGET_NAME} PRIVATE
            -Wall -Wextra -Wpedantic
            -Wshadow           # Variable shadowing
            -Wnon-virtual-dtor # Missing virtual destructors
            -Wold-style-cast   # C-style casts
            -Wcast-align       # Alignment violations
            -Wconversion       # Implicit type conversions
            -Wsign-conversion  # Sign conversion issues
        )
    endif()

    # Treat warnings as errors if defined to do so
    if(PULSE_WARNINGS_AS_ERRORS)
        if(MSVC)
            target_compile_options(${TARGET_NAME} PRIVATE /WX)
        else()
            target_compile_options(${TARGET_NAME} PRIVATE -Werror)
        endif()
    endif()

    # macOS SDK for correct C headers/types
    if(APPLE)
        target_compile_options(${TARGET_NAME} PRIVATE -isysroot ${CMAKE_OSX_SYSROOT})
        target_link_options(${TARGET_NAME} PRIVATE -isysroot ${CMAKE_OSX_SYSROOT})
    endif()

    # Organize targets in IDE folders for better project navigation
    # This uses the module's relative path as the folder structure
    get_target_property(TARGET_SOURCE_DIR ${TARGET_NAME} SOURCE_DIR)
    file(RELATIVE_PATH REL_PATH ${CMAKE_SOURCE_DIR}/projects ${TARGET_SOURCE_DIR})
    get_filename_component(FOLDER_NAME ${REL_PATH} DIRECTORY)
    if(FOLDER_NAME)
        set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "projects/${FOLDER_NAME}")
    endif()
endfunction()

# Function to copy resources from a source directory to the build output
# Useful for shaders, textures, configs, etc.
function(pulse_copy_resources TARGET_NAME RESOURCE_DIR)
    if(EXISTS ${RESOURCE_DIR})
        # Create a custom command to copy resources after the target is built
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${RESOURCE_DIR}
            $<TARGET_FILE_DIR:${TARGET_NAME}>/resources
            COMMENT "Copying resources for ${TARGET_NAME}"
        )
        pulse_message(STATUS "Resources will be copied for ${TARGET_NAME}")
    endif()
endfunction()

# Function to generate export headers for library targets
# This helps create clean public APIs for your modules
function(pulse_generate_export_header TARGET_NAME)
    include(GenerateExportHeader)
    generate_export_header(${TARGET_NAME}
        EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/${TARGET_NAME}.export.h
        PREFIX_NAME PULSE_
    )
    # Make the export header available to the target
    target_include_directories(${TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}/include
    )
endfunction()

pulse_message(STATUS "Utility functions loaded")
