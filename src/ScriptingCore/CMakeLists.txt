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

set (PROJNAME ScriptingCore)
Project (${PROJNAME})
message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
    ${FIREBREATH_CONFIG_DIR}
    ${BOOST_SOURCE_DIR}
    )

file (GLOB TYPES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    APITypes.*
    AutoPtr.*
    variant.*
    vector_list.*
    TypeIDMap.*
)

file (GLOB JSAPI_OBJECTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ./JSAPI*
    BrowserHostWr*
    BrowserObjectAPI.*
    AsyncBrowser*
    *Converter.*
    )

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Converter*
    Factory*
    Test*
    *.py
    )

SOURCE_GROUP(JSAPI FILES ${JSAPI_OBJECTS})

SOURCE_GROUP(Types FILES ${TYPES})

set (SOURCES
    ${TYPES}
    ${JSAPI_OBJECTS}
    ${GENERAL}
    )

add_library(${PROJNAME} STATIC ${SOURCES})
