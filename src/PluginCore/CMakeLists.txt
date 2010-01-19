#/**********************************************************\ 
#Original Author: Richard Bateman (taxilian)
#
#Created:    Nov 20, 2009
#License:    Dual license model; choose one of two:
#            Eclipse Public License - Version 1.0
#            http://www.eclipse.org/legal/epl-v10.html
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#            
#Copyright 2009 PacketPass, Inc and the Firebreath development team
#\**********************************************************/

# Written to work with cmake 2.8
cmake_minimum_required (VERSION 2.8)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.8)

include(${CMAKE_DIR}/common.cmake)

set (PROJNAME PluginCore)
Project (${PROJNAME})
message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
    ${SCRIPTINGCORE_SOURCE_DIR}
    ${BOOST_SOURCE_DIR}
    )

file (GLOB EVENTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    PluginEvents/*.h
    PluginEvents/*.cpp
    )

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp
    *.h
    )

source_group(PluginEvents FILES ${EVENTS})

file (GLOB LINUX_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Linux/*.h
    Linux/*.cpp
    )

SOURCE_GROUP(Linux FILES ${Linux_SOURCES})

file (GLOB WIN_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Win/*.h
    Win/*.cpp
    )

SOURCE_GROUP(Windows FILES ${WIN_SOURCES})

file (GLOB MAC_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac/*.h
    Mac/*.cpp
    )

SOURCE_GROUP(Mac FILES ${MAC_SOURCES})

if (WIN32)
    set_source_files_properties(${MAC_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
    set_source_files_properties(${LINUX_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
endif()
if(APPLE)
    set_source_files_properties(${WIN_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
    set_source_files_properties(${LINUX_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
elseif(UNIX)
    set_source_files_properties(${WIN_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
    set_source_files_properties(${MAC_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
endif()

set (SOURCES
    ${GENERAL}
    ${EVENTS}
    ${WIN_SOURCES}
    ${MAC_SOURCES}
    ${LINUX_SOURCES}
    )

add_library(${PROJNAME} STATIC ${SOURCES})
