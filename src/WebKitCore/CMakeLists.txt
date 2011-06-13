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
#Copyright 2011 Facebook, Inc and the Firebreath development team
#\**********************************************************/

# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

# Currently this project has only been tested on Mac

Project (WebKitCore)
if (VERBOSE)
    message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FB_CONFIG_DIR}
    ${FB_SCRIPTINGCORE_SOURCE_DIR}
    ${FB_PLUGINCORE_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    )

file (GLOB SCRIPTING RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    WebScript*.h
    WebScript*.mm
    JSObject*.h
    JSObject*.cpp
    JSValue*.h
    WebKit*.h
    WebKit*.cpp
)

set (SOURCES
    ${SCRIPTING}
    )

add_library(${PROJECT_NAME} STATIC ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "FireBreath Core")

if (APPLE)
    find_library(WEBKIT WebKit)
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(JAVASCRIPTCORE JavaScriptCore)

    include_directories(${JAVASCRIPTCORE}/Versions/A/Headers)

    target_link_libraries(${PROJECT_NAME}
        ${WEBKIT}
        ${COCOA_FRAMEWORK}
        ${JAVASCRIPTCORE}
        )
endif()
target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    )

export(TARGETS ${PROJECT_NAME} APPEND FILE ${FB_EXPORT_FILE})
if (FB_RELEASE)
    install (TARGETS ${PROJECT_NAME}
        EXPORT FireBreath_Exports
        DESTINATION bin/\${BUILD_TYPE}
        COMPONENT FBCORE
        )
endif()
