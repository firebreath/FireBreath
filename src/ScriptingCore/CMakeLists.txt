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

set (PROJNAME ScriptingCore)
Project (${PROJNAME})
message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GTK_INCLUDE_DIRS}
    ${FIREBREATH_CONFIG_DIR}
    ${Boost_INCLUDE_DIRS}
    )

file (GLOB DOMTYPES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    DOM/[^.]*
)

SOURCE_GROUP(DOM FILES ${DOMTYPES})

file (GLOB UTILTYPES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Util/[^.]*
)

SOURCE_GROUP(Util FILES ${UTILTYPES})

file (GLOB TYPES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    APITypes.*
    AutoPtr.*
    variant.*
    variant_list.*
	  variant_map.*
    TypeIDMap.*
)

file (GLOB JSAPI_OBJECTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    JSAPI*
    BrowserHostWrapper.*
    BrowserObjectAPI.*
    AsyncBrowser*
    *Converter.*
    )

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Converter*
    Factory*
    Test*
    [^.]*.py
    )

SOURCE_GROUP(JSAPI FILES ${JSAPI_OBJECTS})

SOURCE_GROUP(Types FILES ${TYPES})

set (SOURCES
    ${TYPES}
    ${DOMTYPES}
    ${UTILTYPES}
    ${JSAPI_OBJECTS}
    ${GENERAL}
    )

add_library(${PROJNAME} STATIC ${SOURCES})
