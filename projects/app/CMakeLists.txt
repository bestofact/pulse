# Define the main application executable
pulse_collect_sources(MY_SOURCES ${CMAKE_CURRENT_SOURCE_DIR})

# Raylib begin
set(RAYLIB_ROOT "${CMAKE_SOURCE_DIR}/external/raylib")
set(RAYLIB_INCLUDE "${RAYLIB_ROOT}/include")
set(RAYLIB_LIB "${RAYLIB_ROOT}/lib/libraylib.a") # or .dylib
set(RAYLIB_SYS "-framework Cocoa" "-framework IOKit" "-framework OpenGL")
# Raylib end

pulse_add_executable(app
    SOURCES ${MY_SOURCES}
    DEPENDENCIES
        ecs  # Link with the ECS module
    INCLUDE_DIRS
        ${CMAKE_SOURCE_DIR}/projects
        ${RAYLIB_INCLUDE}
    SYSTEM_LIBRARIES
        ${RAYLIB_LIB}
        ${RAYLIB_SYS}
)

# If you want to copy resources (shaders, textures, etc.) to the output directory
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources)
    pulse_copy_resources(app ${CMAKE_CURRENT_SOURCE_DIR}/resources)
endif()

# You can also add custom properties specific to this module
# For example, setting the app as the startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT app)
