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

include(FindCurl)

if (CURL_FOUND)
    list(APPEND FBLIB_INCLUDE_DIRECTORIES
        ${CURL_INCLUDE_DIR}
        )
    list(APPEND FBLIB_LIBRARIES
        ${CURL_LIBRARIES}
        )
    if (WIN32)
        list(APPEND FBLIB_LIBRARIES
            ${CURL_EAY_LIBRARIES}
            )
    endif()
elseif( WIN32 )
    if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/curl)
        find_program(HG "hg" DOC "location of the mercurial executable")
        if (HG MATCHES "HG_NOTFOUND")
            message("Could not find mercurial executable to get the curl libs. "
                "Please get them from http://code.google.com/p/firebreath/source/checkout?repo=curl "
                "and put them in ${CMAKE_CURRENT_SOURCE_DIR}/curl/" FATAL)
        else()
            message("No curl libraries found; fetching from FireBreath codebase")
            execute_process(
                COMMAND ${HG} clone https://curl.firebreath.googlecode.com/hg/ openssl
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
        endif()
    endif()
    list(APPEND FBLIB_LIBRARIES
        ${CMAKE_CURRENT_SOURCE_DIR}/curl/lib/libcurl.lib
        )
    list(APPEND FBLIB_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/curl/include
        )
else()
    message("Could not find Curl libraries!  Please install curl libraries and try again!" FATAL)
endif()

export_project_dependencies()
