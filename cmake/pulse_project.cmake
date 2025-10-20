# Pulse Engine - Project Definition Functions
# This file provides high-level functions for defining Pulse modules

# Include our utility functions
include(${CMAKE_SOURCE_DIR}/cmake/pulse_utility.cmake)

# Main function to define a Pulse module
# This simplifies the process of creating a new module with consistent settings
#
# Usage:
#   pulse_define_module(
#     NAME module_name
#     TYPE [STATIC|SHARED|INTERFACE|EXECUTABLE]
#     SOURCES src/file1.cpp src/file2.cpp
#     HEADERS include/header1.h include/header2.h
#     DEPENDENCIES module1 module2
#     SYSTEM_LIBRARIES OpenGL::GL pthread
#     INCLUDE_DIRS include/
#     PRIVATE_INCLUDE_DIRS src/internal/
#     DEFINITIONS PULSE_DEBUG ENABLE_LOGGING
#   )
function(pulse_define_module)
    # Parse the function arguments using CMake's argument parsing
    set(OPTIONS "")
    set(ONE_VALUE_ARGS NAME TYPE)
    set(MULTI_VALUE_ARGS
        SOURCES
        HEADERS
        DEPENDENCIES
        SYSTEM_LIBRARIES
        INCLUDE_DIRS
        PRIVATE_INCLUDE_DIRS
        DEFINITIONS
    )
    cmake_parse_arguments(MODULE
        "${OPTIONS}"
        "${ONE_VALUE_ARGS}"
        "${MULTI_VALUE_ARGS}"
        ${ARGN}
    )

    # Validate that required arguments are provided
    if(NOT MODULE_NAME)
        message(FATAL_ERROR "pulse_define_module requires NAME argument")
    endif()

    if(NOT MODULE_TYPE)
        # Default to static library if no type is specified
        set(MODULE_TYPE STATIC)
    endif()

    pulse_message(STATUS "Defining module: ${MODULE_NAME} (${MODULE_TYPE})")

    # Create the target based on the specified type
    if(MODULE_TYPE STREQUAL "EXECUTABLE")
        add_executable(${MODULE_NAME} ${MODULE_SOURCES} ${MODULE_HEADERS})
    elseif(MODULE_TYPE STREQUAL "INTERFACE")
        # Interface libraries don't have source files
        add_library(${MODULE_NAME} INTERFACE)
    else()
        # STATIC or SHARED library
        add_library(${MODULE_NAME} ${MODULE_TYPE} ${MODULE_SOURCES} ${MODULE_HEADERS})
    endif()

    # Set up include directories for the module
    # PUBLIC means both the module and its dependents can use these paths
    if(MODULE_INCLUDE_DIRS)
        foreach(INCLUDE_DIR ${MODULE_INCLUDE_DIRS})
            # Support both absolute and relative paths
            if(NOT IS_ABSOLUTE ${INCLUDE_DIR})
                set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_DIR})
            endif()

            if(MODULE_TYPE STREQUAL "INTERFACE")
                target_include_directories(${MODULE_NAME} INTERFACE ${INCLUDE_DIR})
            else()
                target_include_directories(${MODULE_NAME} PUBLIC ${INCLUDE_DIR})
            endif()
        endforeach()
    endif()

    # Set up private include directories (only for this module)
    if(MODULE_PRIVATE_INCLUDE_DIRS AND NOT MODULE_TYPE STREQUAL "INTERFACE")
        foreach(INCLUDE_DIR ${MODULE_PRIVATE_INCLUDE_DIRS})
            if(NOT IS_ABSOLUTE ${INCLUDE_DIR})
                set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_DIR})
            endif()
            target_include_directories(${MODULE_NAME} PRIVATE ${INCLUDE_DIR})
        endforeach()
    endif()

    # Link with other Pulse modules
    if(MODULE_DEPENDENCIES)
        if(MODULE_TYPE STREQUAL "INTERFACE")
            target_link_libraries(${MODULE_NAME} INTERFACE ${MODULE_DEPENDENCIES})
        else()
            target_link_libraries(${MODULE_NAME} PUBLIC ${MODULE_DEPENDENCIES})
        endif()
        pulse_message(STATUS "  Dependencies: ${MODULE_DEPENDENCIES}")
    endif()

    # Link with system/external libraries
    if(MODULE_SYSTEM_LIBRARIES)
        if(MODULE_TYPE STREQUAL "INTERFACE")
            target_link_libraries(${MODULE_NAME} INTERFACE ${MODULE_SYSTEM_LIBRARIES})
        else()
            target_link_libraries(${MODULE_NAME} PRIVATE ${MODULE_SYSTEM_LIBRARIES})
        endif()
        pulse_message(STATUS "  System libraries: ${MODULE_SYSTEM_LIBRARIES}")
    endif()

    # Add preprocessor definitions
    if(MODULE_DEFINITIONS)
        if(MODULE_TYPE STREQUAL "INTERFACE")
            target_compile_definitions(${MODULE_NAME} INTERFACE ${MODULE_DEFINITIONS})
        else()
            target_compile_definitions(${MODULE_NAME} PUBLIC ${MODULE_DEFINITIONS})
        endif()
    endif()

    # Apply common properties to the target (unless it's an interface library)
    if(NOT MODULE_TYPE STREQUAL "INTERFACE")
        pulse_set_common_properties(${MODULE_NAME})
    endif()

    # Store module information for potential later use
    set_property(GLOBAL PROPERTY PULSE_MODULE_${MODULE_NAME}_TYPE ${MODULE_TYPE})
    set_property(GLOBAL APPEND PROPERTY PULSE_ALL_MODULES ${MODULE_NAME})
endfunction()

# Simplified function for creating a basic static library module
# This is a convenience wrapper around pulse_define_module for the common case
#
# Usage:
#   pulse_add_library(my_module
#     SOURCES src/file1.cpp src/file2.cpp
#     DEPENDENCIES other_module
#   )
function(pulse_add_library MODULE_NAME)
    pulse_define_module(
        NAME ${MODULE_NAME}
        TYPE STATIC
        ${ARGN}
    )
endfunction()

# Simplified function for creating an executable module
#
# Usage:
#   pulse_add_executable(my_app
#     SOURCES src/main.cpp
#     DEPENDENCIES engine_core
#   )
function(pulse_add_executable MODULE_NAME)
    pulse_define_module(
        NAME ${MODULE_NAME}
        TYPE EXECUTABLE
        ${ARGN}
    )
endfunction()

# Function to collect all source files in a directory automatically
# This can save time but should be used carefully as it won't trigger
# reconfiguration when new files are added
#
# Usage:
#   pulse_collect_sources(SOURCES_VAR src/)
function(pulse_collect_sources OUT_VAR SOURCE_DIR)
    file(GLOB_RECURSE COLLECTED_SOURCES
        ${SOURCE_DIR}/*.cpp
        ${SOURCE_DIR}/*.c
        ${SOURCE_DIR}/*.cc
    )
    file(GLOB_RECURSE COLLECTED_HEADERS
        ${SOURCE_DIR}/*.h
        ${SOURCE_DIR}/*.hpp
    )
    set(${OUT_VAR} ${COLLECTED_SOURCES} ${COLLECTED_HEADERS} PARENT_SCOPE)
endfunction()

pulse_message(STATUS "Project definition functions loaded")
