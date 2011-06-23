#/**********************************************************\
#Original Author: Richard Bateman (taxilian)
#
#Created:    Jun 23, 2011
#License:    Dual license model; choose one of two:
#            New BSD License
#            http://www.opensource.org/licenses/bsd-license.php
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#
#Copyright 2011 Facebook, Inc
#\**********************************************************/

file (GLOB PLATFORM RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac/[^.]*.cpp
    Mac/[^.]*.mm
    Mac/[^.]*.h
    Mac/[^.]*.cmake
    )

# use this to add preprocessor definitions
add_definitions(
)

SOURCE_GROUP(Mac FILES ${PLATFORM})

set (SOURCES
    ${SOURCES}
    ${PLATFORM}
    )

include_directories(${JAVASCRIPTCORE}/Versions/A/Headers)

message("Adding library WebView")
add_library(WebView STATIC ${SOURCES})

add_subdirectory(${FB_WEBKITCORE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/WebKitCore)

find_library(WEBKIT WebKit)
find_library(JAVASCRIPTCORE JavaScriptCore)

append_firebreath_include_dir(${JAVASCRIPTCORE}/Versions/A/Headers)
append_firebreath_include_dir(${FB_ROOT_DIR}/src/WebKitCore)
append_firebreath_link_library(${WEBKIT})
append_firebreath_link_library(${JAVASCRIPTCORE})
append_firebreath_link_library(WebKitCore)
