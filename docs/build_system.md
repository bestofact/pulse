# Pulse Engine Build System Documentation

## Overview

The Pulse Engine uses a modular CMake build system that automatically discovers and builds all modules in the `projects/` directory. This system is designed to be simple, scalable, and maintainable.

## Quick Start

### 1. Generate the Build System

```bash
# Make the scripts executable (first time only)
chmod +x scripts/generate.sh scripts/build.sh

# Generate with default settings (Release build)
./scripts/generate.sh

# Or generate a Debug build
./scripts/generate.sh -t Debug

# Clean and regenerate
./scripts/generate.sh -c -t Debug
```

### 2. Build the Project

```bash
# Build everything
./scripts/build.sh

# Build with 8 parallel jobs
./scripts/build.sh -j 8

# Build a specific target
./scripts/build.sh -t app

# Clean build with verbose output
./scripts/build.sh -c -v
```

### 3. Run Your Application

After building, executables will be in:
```
build/bin/          # Single-config generators (Make, Ninja)
build/bin/Debug/    # Multi-config generators (Visual Studio, Xcode)
build/bin/Release/
```

## Directory Structure

```
pulse/
├── cmake/
│   ├── pulse_utility.cmake      # Low-level utility functions
│   └── pulse_project.cmake      # High-level module definition functions
├── projects/
│   ├── app/
│   │   ├── CMakeLists.txt
│   │   ├── main.cpp
│   │   └── include/
│   ├── ecs/
│   │   ├── CMakeLists.txt
│   │   ├── entity/
│   │   ├── component/
│   │   └── system/
│   └── [other modules]/
├── scripts/
│   ├── generate.sh              # Project generation script
│   └── build.sh                 # Build script
├── build/                       # Build output (auto-generated)
│   ├── bin/                     # Executables
│   └── lib/                     # Libraries
├── CMakeLists.txt              # Root CMake file
└── .clangd                     # Optional: clangd configuration
```

## Creating a New Module

### Step 1: Create Module Directory

Create a new directory under `projects/`:

```bash
mkdir -p projects/my_module
cd projects/my_module
```

### Step 2: Create CMakeLists.txt

Create a `CMakeLists.txt` file in your module directory:

#### For a Library Module:

```cmake
# Simple static library
pulse_add_library(my_module
    SOURCES
        src/my_class.cpp
        src/another_class.cpp
    HEADERS
        include/my_class.h
        include/another_class.h
    INCLUDE_DIRS
        include/
)
```

#### For an Executable Module:

```cmake
# Application executable
pulse_add_executable(my_app
    SOURCES
        src/main.cpp
    DEPENDENCIES
        my_module  # Link with other Pulse modules
        ecs
    INCLUDE_DIRS
        include/
)
```

#### Advanced Example with Dependencies:

```cmake
# More complex library with dependencies
pulse_add_library(renderer
    SOURCES
        src/renderer.cpp
        src/shader.cpp
        src/texture.cpp
    HEADERS
        include/renderer.h
        include/shader.h
        include/texture.h
    INCLUDE_DIRS
        include/
    PRIVATE_INCLUDE_DIRS
        src/internal/  # Only visible to this module
    DEPENDENCIES
        ecs            # Other Pulse modules
    SYSTEM_LIBRARIES
        OpenGL::GL     # External libraries
        ${CMAKE_DL_LIBS}
    DEFINITIONS
        RENDERER_VERSION="1.0"
        USE_OPENGL_4_5
)

# Find OpenGL package
find_package(OpenGL REQUIRED)
```

### Step 3: Add Your Source Files

Create your source files in the module directory. The structure is flexible:

```
projects/my_module/
├── CMakeLists.txt
├── include/
│   └── my_module/
│       └── my_class.h
└── src/
    └── my_class.cpp
```

### Step 4: Build

The module will be automatically discovered and built:

```bash
./scripts/generate.sh
./scripts/build.sh
```

## Available CMake Functions

### High-Level Functions (from pulse_project.cmake)

#### `pulse_add_library(name ...)`
Creates a static library module.

**Parameters:**
- `SOURCES` - List of source files (.cpp, .c)
- `HEADERS` - List of header files (.h, .hpp)
- `DEPENDENCIES` - Other Pulse modules to link against
- `SYSTEM_LIBRARIES` - External/system libraries to link
- `INCLUDE_DIRS` - Public include directories
- `PRIVATE_INCLUDE_DIRS` - Private include directories
- `DEFINITIONS` - Preprocessor definitions

**Example:**
```cmake
pulse_add_library(core
    SOURCES src/core.cpp
    DEPENDENCIES utils
    INCLUDE_DIRS include/
)
```

#### `pulse_add_executable(name ...)`
Creates an executable.

Uses the same parameters as `pulse_add_library`.

**Example:**
```cmake
pulse_add_executable(game
    SOURCES src/main.cpp
    DEPENDENCIES core renderer
)
```

#### `pulse_define_module(NAME name TYPE type ...)`
Advanced function for full control over module creation.

**TYPE options:**
- `STATIC` - Static library
- `SHARED` - Shared library (DLL/SO)
- `INTERFACE` - Header-only library
- `EXECUTABLE` - Executable program

**Example:**
```cmake
pulse_define_module(
    NAME my_header_only_lib
    TYPE INTERFACE
    HEADERS include/utils.h
    INCLUDE_DIRS include/
)
```

#### `pulse_collect_sources(out_var directory)`
Automatically collects all source files in a directory.

**Example:**
```cmake
pulse_collect_sources(MY_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src)
pulse_add_library(my_module SOURCES ${MY_SOURCES})
```

**Note:** This won't trigger reconfiguration when new files are added. Prefer explicit file lists for better dependency tracking.

### Low-Level Utility Functions (from pulse_utility.cmake)

#### `pulse_message(TYPE message)`
Prints a formatted Pulse-specific message.

```cmake
pulse_message(STATUS "Building renderer module")
pulse_message(WARNING "Deprecated API used")
```

#### `pulse_copy_resources(target resource_dir)`
Copies resources to the output directory after building.

```cmake
pulse_copy_resources(game ${CMAKE_CURRENT_SOURCE_DIR}/assets)
```

#### `pulse_set_common_properties(target)`
Applies common compiler flags and settings. Called automatically by `pulse_add_library` and `pulse_add_executable`.

#### `pulse_generate_export_header(target)`
Generates export macros for shared libraries.

```cmake
if(BUILD_SHARED_LIBS)
    pulse_generate_export_header(my_dll)
endif()
```

## Build Configuration

### Build Types

- **Debug**: No optimization, full debug info
- **Release**: Full optimization, no debug info
- **RelWithDebInfo**: Optimization + debug info
- **MinSizeRel**: Optimize for size

```bash
./scripts/generate.sh -t Debug
./scripts/generate.sh -t Release
```

### Using Different Generators

```bash
# Ninja (fast parallel builds)
./scripts/generate.sh -g Ninja

# Unix Makefiles (default on Linux/Mac)
./scripts/generate.sh -g "Unix Makefiles"

# Visual Studio (Windows)
./scripts/generate.sh -g "Visual Studio 17 2022"

# Xcode (macOS)
./scripts/generate.sh -g Xcode
```

### Custom Build Directory

```bash
# Generate in a custom location
./scripts/generate.sh -b build-debug -t Debug
./scripts/build.sh -b build-debug

# Have multiple build directories for different configs
./scripts/generate.sh -b build-debug -t Debug
./scripts/generate.sh -b build-release -t Release
```

## Advanced Topics

### Module Dependencies

Dependencies are automatically handled. Just list them in `DEPENDENCIES`:

```cmake
pulse_add_library(graphics
    SOURCES src/graphics.cpp
    DEPENDENCIES
        core      # Will be linked automatically
        math
)

pulse_add_executable(game
    SOURCES src/main.cpp
    DEPENDENCIES
        graphics  # This will also pull in core and math
)
```

### External Libraries

#### Using find_package:

```cmake
find_package(Threads REQUIRED)

pulse_add_library(my_module
    SOURCES src/file.cpp
    SYSTEM_LIBRARIES Threads::Threads
)
```

#### Using pkg-config:

```cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

pulse_add_library(window
    SOURCES src/window.cpp
    SYSTEM_LIBRARIES ${SDL2_LIBRARIES}
    INCLUDE_DIRS ${SDL2_INCLUDE_DIRS}
)
```

### Conditional Compilation

```cmake
# Platform-specific code
if(WIN32)
    set(PLATFORM_SOURCES src/platform_windows.cpp)
elseif(UNIX)
    set(PLATFORM_SOURCES src/platform_unix.cpp)
endif()

pulse_add_library(platform
    SOURCES
        src/common.cpp
        ${PLATFORM_SOURCES}
    INCLUDE_DIRS include/
)
```

### Custom Build Options

Add options to your module's CMakeLists.txt:

```cmake
option(ENABLE_LOGGING "Enable debug logging" ON)
option(USE_DOUBLE_PRECISION "Use double precision floats" OFF)

set(MY_DEFINITIONS "")
if(ENABLE_LOGGING)
    list(APPEND MY_DEFINITIONS ENABLE_LOGGING)
endif()

pulse_add_library(my_module
    SOURCES src/file.cpp
    DEFINITIONS ${MY_DEFINITIONS}
)
```

Then toggle them:

```bash
./scripts/generate.sh -t Release
cmake -DENABLE_LOGGING=OFF build/
./scripts/build.sh
```

## Troubleshooting

### Module Not Found

**Problem:** New module isn't being built.

**Solution:** Make sure:
1. Module directory is in `projects/`
2. Module has a `CMakeLists.txt` file
3. You've run `generate.sh` after adding the module

### Linking Errors

**Problem:** Undefined references during linking.

**Solution:**
1. Check that dependencies are listed in `DEPENDENCIES`
2. Verify dependency order (dependencies should be listed after dependents)
3. Make sure header files match the implementation

### Include Path Issues

**Problem:** Headers not found during compilation.

**Solution:**
1. Add the include directory to `INCLUDE_DIRS`
2. Check that the path is relative to the module's CMakeLists.txt
3. Use `PRIVATE_INCLUDE_DIRS` for internal headers

### Clean Build

If you encounter strange issues, try a clean build:

```bash
./scripts/generate.sh -c
./scripts/build.sh -c
```

Or manually:

```bash
rm -rf build/
./scripts/generate.sh
./scripts/build.sh
```

## Tips and Best Practices

1. **Explicit is Better**: List source files explicitly rather than using globbing
2. **Organize by Feature**: Group related functionality into modules
3. **Minimize Dependencies**: Keep modules loosely coupled
4. **Use Interface Libraries**: For header-only utilities
5. **Version Control**: Don't commit the `build/` directory
6. **Parallel Builds**: Use `-j` flag for faster builds
7. **IDE Integration**: Generate IDE-specific projects when needed

## IDE Setup

### Visual Studio Code

Install the CMake Tools extension and it will automatically detect the build system.

### CLion

CLion has native CMake support. Just open the project root directory.

### Visual Studio

Generate Visual Studio solution files:

```bash
./scripts/generate.sh -g "Visual Studio 17 2022"
```

Then open `build/PulseEngine.sln`.

### Xcode

Generate Xcode project:

```bash
./scripts/generate.sh -g Xcode
```

Then open `build/PulseEngine.xcodeproj`.