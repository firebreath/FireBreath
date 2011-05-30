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

project(FireBreath_Cmake)

file (GLOB COMMON RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    common.cmake
    CommonPluginConfig.cmake
    configure_template.cmake
    paths.cmake
    buildconfig.cmake
    CMakeLists.txt
    dummy.cpp
    options.cmake
    PluginConfigDefaults.cmake
    pluginProjects.cmake
    README.txt
    )

SOURCE_GROUP(Common FILES ${COMMON})

file (GLOB PLATFORM_WIN RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac.cmake
    patch_xcode.py
    )
SOURCE_GROUP(Windows FILES ${PLATFORM_WIN})
file (GLOB PLATFORM_MAC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Win.cmake
    wix.cmake
    FixFragment.xslt
    )
SOURCE_GROUP(Mac FILES ${PLATFORM_MAC})
file (GLOB PLATFORM_X11 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    X11.cmake
    )
SOURCE_GROUP(X11 FILES ${PLATFORM_X11})

set (SOURCES
    ${COMMON}
    ${PLATFORM_WIN}
    ${PLATFORM_MAC}
    ${PLATFORM_X11}
    )

add_library(${PROJECT_NAME} STATIC ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "FireBreath Core")
