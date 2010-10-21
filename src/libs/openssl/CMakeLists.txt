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

file (GLOB GENERAL_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    include/openssl/*.h
)

if (WIN32)
    list(APPEND FBLIB_LIBRARIES
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/libeay32.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/ssleay32.lib
        )
    list(APPEND FBLIB_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )
elseif (UNIX)
    if (APPLE)
        set(platform mac)
    else()
        set(platform linux)
    endif()
endif()

export_project_dependencies()
