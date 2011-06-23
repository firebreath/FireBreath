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
    Win/[^.]*.cpp
    Win/[^.]*.h
    Win/[^.]*.cmake
    )

# use this to add preprocessor definitions
add_definitions(
)

SOURCE_GROUP(Win FILES ${PLATFORM})

set (SOURCES
    ${SOURCES}
    ${PLATFORM}
    )

message("Adding library WebView")
add_library(WebView STATIC ${SOURCES})

append_firebreath_include_dir(${FB_ACTIVEXCORE_SOURCE_DIR})
