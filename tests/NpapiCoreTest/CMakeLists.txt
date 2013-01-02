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

Project (UnitTest_NpapiCore)
if (VERBOSE)
    message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")
endif()

include_directories(
    ${GECKOSDK_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${NPAPICORE_SOURCE_DIR}
    ${NPAPIHOST_SOURCE_DIR}
    ${SCRIPTINGCORE_SOURCE_DIR}
    ${PLUGINCORE_SOURCE_DIR}
    ${PLUGINAUTO_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${UNITTEST++_SOURCE_DIR}/src
    ${FB_CONFIG_DIR}
    )

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ./[^.]*.h
    ./[^.]*.cpp
    )

set (SOURCES
    ${GENERAL}
    ${FB_PLUGINAUTO_SOURCE_DIR}/null/NullLogger.cpp
    )

add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "UnitTests")

if (APPLE)
    if (FBMAC_USE_CARBON)
        find_library(FRAMEWORK_CARBON Carbon)
    endif (FBMAC_USE_CARBON)
    find_library(FRAMEWORK_COCOA Cocoa)
    find_library(FRAMEWORK_FOUNDATION Foundation)
    find_library(FRAMEWORK_APPLICATIONSERVICES ApplicationServices)
    find_library(FRAMEWORK_QUARTZCORE QuartzCore)
    set (OTHER_NPAPITEST_LIBS 
        ${OTHER_NPAPITEST_LIBS} 
        ${FRAMEWORK_COCOA}
        ${FRAMEWORK_FOUNDATION}
        ${FRAMEWORK_APPLICATIONSERVICES}
        ${FRAMEWORK_QUARTZCORE}
        ${FRAMEWORK_CARBON})
elseif (UNIX)
    if (NOT FB_GUI_DISABLED)
        set(OTHER_NPAPITEST_LIBS 
            ${GTK_LIBRARIES})
    endif()
endif (APPLE)

# Repititions in the following are intentional to fix linking errors due to
# cyclic references on Linux. Don't change without testing on Linux!
target_link_libraries (${PROJECT_NAME}
    ${Boost_LIBRARIES}
    PluginCore
    NpapiCore
    ScriptingCore
    NPAPIHost
    UnitTest++
    ${OTHER_NPAPITEST_LIBS}
    )
