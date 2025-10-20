cmake_minimum_required(VERSION 3.26)
project(pulse LANGUAGES CXX)

# Set the default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
    # Set the possible values for cmake-gui and ccmake
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Report" "Release" "Publish")
endif()

# Output directories - this keeps your build organized
# All executables will go to the bin folder, libraries to lib
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Ensure output directories are created for multi-config generators (like Visual Studio)
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin/${CONFIG})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib/${CONFIG})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib/${CONFIG})
endforeach()

# Add the cmake directory to the module path so we can include our custom scripts
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Load our custom CMake functions
include(pulse_utility)
include(pulse_project)
include(pulse_toolchain)

# Global compiler settings for the entire project
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF) # avoid gnu++ mode

# Enable folder organization in IDEs (Visual Studio, Xcode, etc.)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Print configuration information for debugging
pulse_message(STATUS "========================================")
pulse_message(STATUS "Pulse Engine Configuration")
pulse_message(STATUS "========================================")
pulse_message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
pulse_message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
pulse_message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
pulse_message(STATUS "Binary directory: ${CMAKE_BINARY_DIR}")
pulse_message(STATUS "========================================")


# Automatically discover and include all modules in the projects directory
# This is the magic that makes your build system automatically pick up new modules
pulse_message(STATUS "Discovering modules in projects/")
pulse_collect_subdirectories(PROJECT_MODULES ${CMAKE_SOURCE_DIR}/projects)

if(PROJECT_MODULES)
    list(LENGTH PROJECT_MODULES PROJECT_MODULE_COUNT)
    pulse_message(STATUS "Found ${PROJECT_MODULES_COUNT} modules:")
    foreach(MODULE_DIR ${PROJECT_MODULES})
        pulse_message(STATUS "  - ${MODULE_DIR}")
        add_subdirectory(projects/${MODULE_DIR})
    endforeach()
else()
    pulse_message(WARNING "No modules found in projects/ directory")
endif()

# Optional: Print summary of all discovered modules at the end
get_property(ALL_MODULES GLOBAL PROPERTY PULSE_ALL_MODULES)
if(ALL_MODULES)
    pulse_message(STATUS "========================================")
    pulse_message(STATUS "Build Summary")
    pulse_message(STATUS "========================================")
    foreach(MODULE ${ALL_MODULES})
        get_property(MODULE_TYPE GLOBAL PROPERTY PULSE_MODULE_${MODULE}_TYPE)
        pulse_message(STATUS "  ${MODULE} (${MODULE_TYPE})")
    endforeach()
    pulse_message(STATUS "========================================")
endif()

# Create a symlink to compile_commands.json in the source directory
# This makes it easy for tools like clangd to find the compilation database
# Only create the symlink on Unix-like systems (not Windows)
if(UNIX AND CMAKE_EXPORT_COMPILE_COMMANDS)
    set(COMPILE_COMMANDS_SOURCE "${CMAKE_BINARY_DIR}/compile_commands.json")
    set(COMPILE_COMMANDS_LINK "${CMAKE_SOURCE_DIR}/compile_commands.json")

    # Create a custom target that updates the symlink whenever CMake configures
    add_custom_target(compile_commands_link ALL
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${COMPILE_COMMANDS_SOURCE}
            ${COMPILE_COMMANDS_LINK}
        COMMENT "Creating symlink to compile_commands.json in project root"
        VERBATIM
    )

    pulse_message(STATUS "Compile commands will be symlinked to ${COMPILE_COMMANDS_LINK}")
endif()

# Optional: Add a custom target to format all code using clang-format
find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
    file(GLOB_RECURSE ALL_SOURCE_FILES
       ${CMAKE_SOURCE_DIR}/projects/*.cpp
       ${CMAKE_SOURCE_DIR}/projects/*.h
       ${CMAKE_SOURCE_DIR}/projects/*.hpp
   )
    add_custom_target(format
       COMMAND ${CLANG_FORMAT} -i ${ALL_SOURCE_FILES}
       COMMENT "Running clang-format on all source files"
   )
endif()
