#/**********************************************************\ 
#Original Author: Richard Bateman (taxilian)
#
#Created:    Nov 20, 2009
#License:    Dual license model; choose one of two:
#            New BSD License
#            http://www.opensource.org/licenses/bsd-license.php
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#            
#Copyright 2009 PacketPass, Inc and the Firebreath development team
#\**********************************************************/

# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

include(${CMAKE_DIR}/common.cmake)
include(${CMAKE_CURRENT_BINARY_DIR}/../PluginConfig.cmake)

set (PROJNAME ${PLUGIN_PREFIX}_UnitTest_NpapiPlugin)
Project (${PROJNAME})
if (VERBOSE)
    message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")
endif()

include_directories(
    ${GECKOSDK_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${NPAPIPLUGIN_SOURCE_DIR}
    ${NPAPIHOST_SOURCE_DIR}
    ${SCRIPTINGCORE_SOURCE_DIR}
    ${PLUGINCOMMON_SOURCE_DIR}
    ${GEN_DIR}/gen
    ${Boost_INCLUDE_DIRS}
    ${UNITTEST_FW_SOURCE_DIR}/src
    )

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ./[^.]*.h
    ./[^.]*.cpp
    )

set (SOURCES
    ${GENERAL}
    ${PLUGINCOMMON_SOURCE_DIR}/null/NullLogger.cpp
    )

add_executable(${PROJNAME} ${SOURCES})

add_dependencies(${PROJNAME}
    ScriptingCore
    PluginCore
    NPAPIHost
    ${PLUGIN_PREFIX}_NpapiPlugin
    ${PLUGIN_PREFIX}_PluginWindow
    ${PLUGIN_PREFIX}_PluginCommon
    UnitTest++
    )

if (APPLE)
    if (FBMAC_USE_CARBON)
        find_library(CARBON_FRAMEWORK Carbon)
    endif (FBMAC_USE_CARBON)
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(FOUNDATION_FRAMEWORK Foundation)
    find_library(APPLICATIONSERVICES_FRAMEWORK ApplicationServices)
    find_library(QUARTZCORE_FRAMEWORK QuartzCore)
    set (OTHER_NPAPITEST_LIBS 
        ${OTHER_NPAPITEST_LIBS} 
        ${COCOA_FRAMEWORK}
        ${FOUNDATION_FRAMEWORK}
        ${APPLICATIONSERVICES_FRAMEWORK}
        ${QUARTZCORE_FRAMEWORK}
        ${CARBON_FRAMEWORK})
elseif (UNIX)
    if (NOT GTK_INCLUDE_DIRS)
        pkg_check_modules(GTK REQUIRED gtk+-2.0)
        set (GTK_INCLUDE_DIRS ${GTK_INCLUDE_DIRS} CACHE INTERNAL "GTK include dirs")
        set (GTK_LIBRARIES ${GTK_LIBRARIES} CACHE INTERNAL "GTK include dirs")
        set (GTK_LIBRARY_DIRS ${GTK_LIBRARY_DIRS} CACHE INTERNAL "GTK include dirs")
        set (GTK_LDFLAGS ${GTK_LDFLAGS} CACHE INTERNAL "GTK include dirs")
    endif()
    set (OTHER_NPAPITEST_LIBS ${OTHER_NAPITEST_LIBS} ${GTK_LIBRARIES})
endif (APPLE)

# Repititions in the following are intentional to fix linking errors due to
# cyclic references on Linux. Don't change without testing on Linux!
target_link_libraries (${PROJNAME}
    ${Boost_LIBRARIES}
    PluginCore
    ${PLUGIN_PREFIX}_NpapiPlugin
    ${PLUGIN_PREFIX}_PluginWindow
    PluginCore
    ScriptingCore
    ${PLUGIN_PREFIX}_PluginCommon
    NPAPIHost
    UnitTest++
    PluginCore
    ${OTHER_NPAPITEST_LIBS}
    )
