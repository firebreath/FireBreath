#/**********************************************************\ 
# Auto-generated CMakeLists.txt for the FBTestPlugin project
#\**********************************************************/

# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

include(PluginConfig.cmake)

Project (${PLUGIN_NAME})

set (FB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/firebreath CACHE PATH "Path to FireBreath Root")

add_subdirectory(${FB_ROOT} ${CMAKE_CURRENT_BINARY_DIR}/firebreath)
#include(${FB_ROOT}/PluginIncludes.cmake)

#Visual Studio Express editions don't want Solution Folders enabled.
if(NOT ATL_LIBRARY)
    set_property(GLOBAL PROPERTY USE_FOLDERS On)
endif()

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp
    *.h
    *.cmake
    *.html
    )

include_directories(${PLUGIN_INCLUDE_DIRS})

# Generated files are stored in ${GENERATED} by the project configuration
SET_SOURCE_FILES_PROPERTIES(
    ${GENERATED}
    PROPERTIES
        GENERATED 1
    )

SOURCE_GROUP(Generated FILES
    ${GENERATED}
    )

SET( SOURCES
    ${GENERAL}
    ${GENERATED}
    )

if (WIN32)
    set (PLATFORM_NAME "Win")
elseif(APPLE)
    set (PLATFORM_NAME "Mac")
elseif(UNIX)
    set (PLATFORM_NAME "X11")
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_NAME}/projectDef.cmake)
    #include(${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_NAME}/projectDef.cmake)
else()
    message ("Could not find a ${PLATFORM_NAME} directory for the project")
endif()
