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
        find_program(GIT git PATHS "$ENV{ProgramFiles(x86)}/git/bin" "$ENV{ProgramFiles}/git/bin" DOC "location of the git executable")
        if (GIT)
            execute_process(
                COMMAND ${GIT}
                clone git://github.com/taxilian/firebreath-curl curl
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
        else()            
            message("error ${GIT} to get the curl libs. "
                "Please get them from http://github.com/taxilian/firebreath-curl "
                "and put them in ${CMAKE_CURRENT_SOURCE_DIR}/curl/" FATAL)
        endif()
    endif()
    append_firebreath_link_library(${CMAKE_CURRENT_SOURCE_DIR}/curl/lib/libcurl.lib)
    append_firebreath_include_dir(${CMAKE_CURRENT_SOURCE_DIR}/curl/include)
    add_firebreath_definition(-DCURL_STATICLIB)
else()
    message("Could not find Curl libraries!  Please install curl libraries and try again!" FATAL)
endif()

if (FIREBREATH)
    export_project_dependencies()
endif()
