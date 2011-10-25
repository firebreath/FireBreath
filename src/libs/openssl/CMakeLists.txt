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

include(FindOpenSSL)

if (OPENSSL_FOUND)
    if (FIREBREATH)
        append_firebreath_include_dir("${OPENSSL_INCLUDE_DIR}")
        append_firebreath_link_library("${OPENSSL_LIBRARIES}")
    endif()
    if (WIN32 AND FIREBREATH)
        append_firebreath_link_library("${OPENSSL_EAY_LIBRARIES}")
    endif()
elseif( WIN32 )
    if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/openssl)
        find_program(GIT git PATHS "$ENV{ProgramFiles(x86)}/git/bin" "$ENV{ProgramFiles}/git/bin" DOC "location of the git executable")
        if (GIT)
            execute_process(
                COMMAND ${GIT}
                clone git://github.com/taxilian/firebreath-openssl openssl
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
        else()            
            message("error ${GIT} to get the openssl libs. "
                "Please get them from http://github.com/taxilian/firebreath-openssl "
                "and put them in ${CMAKE_CURRENT_SOURCE_DIR}/openssl/" FATAL)
        endif()
    endif()
    if (FIREBREATH)
        append_firebreath_link_library(${CMAKE_CURRENT_SOURCE_DIR}/openssl/lib/libeay32.lib)
        append_firebreath_link_library(${CMAKE_CURRENT_SOURCE_DIR}/openssl/lib/ssleay32.lib)
        append_firebreath_include_dir(${CMAKE_CURRENT_SOURCE_DIR}/openssl/include)
    endif()
else()
    message("Could not find OpenSSL libraries!  Please install openssl libraries and try again!" FATAL)
endif()

if (FIREBREATH)
    export_project_dependencies()
endif()
