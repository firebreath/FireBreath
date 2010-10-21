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

add_boost_library(thread)
add_boost_library(filesystem)
add_firebreath_library(openssl)

include_directories (
    ${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
    ${SCRIPTINGCORE_SOURCE_DIR}
    ${PLUGINCOMMON_SOURCE_DIR}
    ${FBLIB_INCLUDE_DIRS}
    ${PLUGINWINDOW_SOURCE_DIR}
)

file (GLOB GENERAL_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp
    *.h
)

file (GLOB HTTP_SOURCE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    HTTP*.cpp
    HTTP*.h
)

SOURCE_GROUP(HttpLibs FILES ${HTTP_SOURCE})

if (WIN32)
    set(platform windows)
elseif (UNIX)
    if (APPLE)
        set(platform mac)
    else()
        set(platform linux)
    endif()
endif()

file (GLOB PLATFORM_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Platform/*.h
    Platform/Platform_common.cpp
    Platform/Platform_${platform}.cpp
)

SOURCE_GROUP(Platform FILES ${PLATFORM_SRC})

list(APPEND SOURCES
    ${GENERAL_SRC}
    ${PLATFORM_SRC}
)

add_library (HttpService STATIC ${SOURCES})

append_firebreath_link_library(HttpService)
append_firebreath_link_library(Wininet)
append_firebreath_include_dir(${CMAKE_CURRENT_SOURCE_DIR})

export_project_dependencies()
