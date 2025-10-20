# Define the main application executable
pulse_collect_sources(MY_SOURCES ${CMAKE_CURRENT_SOURCE_DIR})
pulse_add_executable(app
    SOURCES ${MY_SOURCES}
    DEPENDENCIES
        ecs  # Link with the ECS module
    INCLUDE_DIRS
        ${CMAKE_SOURCE_DIR}/projects
)

# If you want to copy resources (shaders, textures, etc.) to the output directory
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources)
    pulse_copy_resources(app ${CMAKE_CURRENT_SOURCE_DIR}/resources)
endif()

# You can also add custom properties specific to this module
# For example, setting the app as the startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT app)
