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

set (PROJNAME NPAPIHost)
Project (${PROJNAME})
if (VERBOSE)
    message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FIREBREATH_CONFIG_DIR}
    ${Boost_INCLUDE_DIRS}
    ${SCRIPTINGCORE_SOURCE_DIR}
    ${ATL_INCLUDE_DIRS}
    ${PLUGINCORE_SOURCE_DIR}
    ${NPAPICORE_SOURCE_DIR}
    ${GECKOSDK_SOURCE_DIR}
    ${CONFIG_DIR}
    .
    )

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    [^.]*.cpp
    [^.]*.h
    )

set (SOURCES
    ${GENERAL}
    )

add_library(${PROJNAME} STATIC ${SOURCES})
