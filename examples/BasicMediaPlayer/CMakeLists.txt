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

Project (${PLUGIN_NAME})

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp
    *.h
    *.cmake
	*.html
    )

include_directories(${PLUGIN_INCLUDE_DIRS})

file (GLOB GENERATED RELATIVE ${CMAKE_CURRENT_BINARY_DIR}
    ${FB_TEMPLATE_DEST_DIR}/*.rgs
    ${FB_TEMPLATE_DEST_DIR}/*.def
    ${FB_TEMPLATE_DEST_DIR}/*.htm
    ${FB_TEMPLATE_DEST_DIR}/*.rc
    )

SET_SOURCE_FILES_PROPERTIES(
    ${GENERATED}
    PROPERTIES
        GENERATED 1
    )

SOURCE_GROUP(Generated FILES
    ${GENERATED}
    )

SET( SOURCES
    ${GENERAL}
    ${GENERATED}
    )

include_platform()
