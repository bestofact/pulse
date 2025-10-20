# Prefix where you installed the fork
set(CLANG_P2996_PREFIX "$ENV{HOME}/opt/clang-p2996" CACHE PATH "Bloomberg P2996 prefix")

# Use the forked compilers
set(CMAKE_C_COMPILER   "${CLANG_P2996_PREFIX}/bin/clang")
set(CMAKE_CXX_COMPILER "${CLANG_P2996_PREFIX}/bin/clang++")

# macOS SDK (sysroot) — critical so libc++ sees the correct C headers
if(APPLE)
  if(NOT DEFINED CMAKE_OSX_SYSROOT OR CMAKE_OSX_SYSROOT STREQUAL "")
    execute_process(
      COMMAND xcrun --show-sdk-path
      OUTPUT_VARIABLE SDK_PATH
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CMAKE_OSX_SYSROOT "${SDK_PATH}" CACHE PATH "" FORCE)
  endif()

  # Let the built binary find the fork's libraries at runtime
  set(CMAKE_INSTALL_RPATH "${CLANG_P2996_PREFIX}/lib")
  set(CMAKE_BUILD_RPATH   "${CLANG_P2996_PREFIX}/lib")
  set(CMAKE_MACOSX_RPATH ON)
endif()
