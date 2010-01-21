#/**********************************************************\ 
# Auto-generated X11 project definition file for the
# @{PLUGIN.name} project
#\**********************************************************/

# Windows template platform definition CMake file
# Included from ../CMakeLists.txt

# remember that the current source dir is the project root; this file is in ${PLATFORM_NAME}/
file (GLOB PLATFORM RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${PLATFORM_NAME}/*.cpp
    ${PLATFORM_NAME}/*.h
    ${PLATFORM_NAME}/*.cmake
    )

SOURCE_GROUP(${PLATFORM_NAME} FILES ${PLATFORM})

# use this to add preprocessor definitions
add_definitions(
    -D"FB_X11=1"
)

set (SOURCES
    ${SOURCES}
    ${PLATFORM}
    )

add_library(${PROJNAME} SHARED ${SOURCES})

set_target_properties (${PROJNAME} PROPERTIES
    OUTPUT_NAME np${PLUGIN_NAME}
    PROJECT_LABEL ${PROJNAME}
    RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}/${PLUGIN_NAME}"
    LIBRARY_OUTPUT_DIRECTORY "${BIN_DIR}/${PLUGIN_NAME}"
    )

# add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
target_link_libraries(${PROJNAME}
    ${PLUGIN_INTERNAL_DEPS}
    )

add_dependencies(${PROJNAME}
    ${PLUGIN_INTERNAL_DEPS}
    )
