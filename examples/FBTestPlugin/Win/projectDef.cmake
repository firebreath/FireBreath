#/**********************************************************\ 
# Auto-generated Windows project definition file for the
# FBTestPlugin project
#\**********************************************************/

# Windows template platform definition CMake file
# Included from ../CMakeLists.txt

# remember that the current source dir is the project root; this file is in Win/
file (GLOB PLATFORM RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Win/*.cpp
    Win/*.h
    Win/*.cmake
    )

# use this to add preprocessor definitions
add_definitions(
    /D "_ATL_STATIC_REGISTRY"
)

SOURCE_GROUP(Win FILES ${PLATFORM})

set (SOURCES
    ${SOURCES}
    ${PLATFORM}
    )

add_windows_plugin(${PROJECT_NAME} SOURCES)

# add library dependencies here; leave ${FB_PLUGIN_LIBRARIES} there unless you know what you're doing!
target_link_libraries(${PROJECT_NAME}
    ${FB_PLUGIN_LIBRARIES}
    )

set(WIX_HEAT_FLAGS
    -gg                 # Generate GUIDs
    -srd                # Suppress Root Dir
    -cg PluginDLLGroup  # Set the Component group name
    -dr INSTALLDIR      # Set the directory ID to put the files in
    )

get_plugin_path(PLUGIN_FILEPATH ${PROJECT_NAME})
get_filename_component(PLUGIN_PATH ${PLUGIN_FILEPATH} DIRECTORY)

add_wix_installer( ${PLUGIN_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/Win/WiX/GCPlugin2Installer.wxs
    PluginDLLGroup
    ${PLUGIN_PATH}
    $<TARGET_FILE:${PROJECT_NAME}>
    ${PROJECT_NAME}
    )
