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

if (WIN32)
    file (GLOB WIN_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        Win/*.h
        Win/*.cpp
        )

    source_group(Windows FILES ${WIN_SOURCES})
endif()

set (SOURCES
    ${GENERAL}
    ${EVENTS}
    ${WIN_SOURCES}
    )

add_library(${PROJNAME} STATIC ${SOURCES})
