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

Project (PluginCore)
if (VERBOSE)
    message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GTK_INCLUDE_DIRS}
    ${FB_SCRIPTINGCORE_SOURCE_DIR}
    ${FB_PLUGINAUTO_SOURCE_DIR}
    ${FB_NPAPICORE_SOURCE_DIR}
    ${FB_ACTIVEXCORE_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${ATL_INCLUDE_DIRS}
    ${FB_CONFIG_DIR}
    )

file (GLOB EVENTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    PluginEvents/[^.]*.h
    PluginEvents/[^.]*.cpp
    )

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    [^.]*.cpp
    [^.]*.h
    )

source_group(PluginEvents FILES ${EVENTS})

file (GLOB X11_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    X11/[^.]*.h
    X11/[^.]*.cpp
    )

SOURCE_GROUP(X11 FILES ${X11_SOURCES})

file (GLOB WIN_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Win/[^.]*.h
    Win/[^.]*.cpp
    )

SOURCE_GROUP(Windows FILES ${WIN_SOURCES})

file (GLOB MAC_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac/[^.]*.h
    Mac/[^.]*.cpp
    Mac/[^.]*.mm
    )

SOURCE_GROUP(Mac FILES ${MAC_SOURCES})

if (WIN32)
    set_source_files_properties(${MAC_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
    set_source_files_properties(${X11_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
endif()
if(APPLE)
    set_source_files_properties(${WIN_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
    set_source_files_properties(${X11_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
elseif(UNIX)
    set_source_files_properties(${WIN_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
    set_source_files_properties(${MAC_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
endif()

set (SOURCES
    ${GENERAL}
    ${EVENTS}
    ${WIN_SOURCES}
    ${MAC_SOURCES}
    ${X11_SOURCES}
    )

if (APPLE)
    FIND_LIBRARY(FOUNDATION NAMES foundation)
    include_directories(${FOUNDATION}/Headers)
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCES})
ADD_PRECOMPILED_HEADER(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/precompiled_headers.h" "${CMAKE_CURRENT_SOURCE_DIR}/precompiled_headers.cpp" SOURCES)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "FireBreath Core")

export(TARGETS PluginCore APPEND FILE ${FB_EXPORT_FILE})

if (FB_RELEASE)
    install (TARGETS PluginCore
        EXPORT FireBreath_Exports
        DESTINATION bin/\${BUILD_TYPE}
        COMPONENT FBCORE
        )
endif()
