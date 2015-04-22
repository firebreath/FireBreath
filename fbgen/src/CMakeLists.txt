#/**********************************************************\ 
# 
# Auto-generated CMakeLists.txt for the @{PLUGIN_name} project
#
#\**********************************************************/

# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

include(PluginConfig.cmake)
Project(${PLUGIN_NAME})

set (FB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/firebreath CACHE PATH "Path to FireBreath Root")
if (NOT EXISTS ${FB_ROOT}/CMakeLists.txt)
    message(FATAL_ERROR "You need to configure FB_ROOT to the root directory of your FireBreath installation!")
endif()

include(${FB_ROOT}/PluginIncludes.cmake)
add_subdirectory(${FB_PLUGINAUTO_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/PluginAuto)

add_subdirectory(${FB_ROOT} ${CMAKE_CURRENT_BINARY_DIR}/firebreath)

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

include_directories(${FB_PLUGIN_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

# Generated files are stored in ${GENERATED} by the project configuration
SET_SOURCE_FILES_PROPERTIES(
    ${FB_PLUGIN_GENERATED_SOURCES}
    PROPERTIES
        GENERATED 1
    )

SOURCE_GROUP(Generated FILES
    ${FB_PLUGIN_GENERATED_SOURCES}
    )

SET( SOURCES
    ${GENERAL}
    ${FB_PLUGIN_GENERATED_SOURCES}
    )

add_firebreath_library(log4cplus)

if (WIN32)
    set (PLATFORM_NAME "Win")
    include(Win/projectDef.cmake)
elseif(APPLE)
    set (PLATFORM_NAME "Mac")
    include(Mac/projectDef.cmake)
elseif(UNIX)
    set (PLATFORM_NAME "X11")
    include(X11/projectDef.cmake)
endif()

configure_chrome_wyrmhole()
add_native_message_host(${PROJECT_NAME})

#this will build a XPI package using XPISigner (see http://code.google.com/p/xpisigner/ )
# 
# add_signed_xpi_installer(${PLUGIN_NAME}
# 	${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/ 
#   "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/${FBSTRING_PluginFileName}.dll"
# 	$ENV{XPI_PATH} 
# 	"${CMAKE_CURRENT_SOURCE_DIR}/sign/certificate.pfx" 
# 	"${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
# 	${PROJECT_NAME})

