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

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/LeakFinder)
    #if the LeakFinder directory is there, use it
    message("-- Found LeakFinder")
    file (GLOB LEAKFINDER RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        LeakFinder/*
        )
    set (SOURCES
        ${SOURCES}
        ${LEAKFINDER}
        )
    source_group(LeakFinder FILES ${LEAKFINDER})
    add_definitions(-DHAS_LEAKFINDER)
endif()

add_windows_plugin(${PROJECT_NAME} SOURCES)

# add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
target_link_libraries(${PROJECT_NAME}
    ${PLUGIN_INTERNAL_DEPS}
    )

set(WIX_HEAT_FLAGS
    -gg                 # Generate GUIDs
    -srd                # Suppress Root Dir
    -cg PluginDLLGroup  # Set the Component group name
    -dr INSTALLDIR      # Set the directory ID to put the files in
    )

add_wix_installer( ${PLUGIN_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/Win/WiX/FBTestPluginInstaller.wxs
    PluginDLLGroup
    ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
    ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/${FBSTRING_PluginFileName}.dll
    ${PROJECT_NAME}
    )
