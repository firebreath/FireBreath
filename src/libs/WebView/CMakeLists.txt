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

# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

project(WebView)
message("Loading FireBreath library WebView")

get_target_property(library_target_exists ${PROJECT_NAME} TYPE)
if (library_target_exists)
    set(TARGET_ALREADY_EXISTS 1)
else()
    set(TARGET_ALREADY_EXISTS 0)
endif()

include_directories (
    ${FB_GECKOSDK_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FB_NPAPICORE_SOURCE_DIR}
    ${FB_SCRIPTINGCORE_SOURCE_DIR}
    ${FB_PLUGINAUTO_SOURCE_DIR}
    ${FB_PLUGINCORE_SOURCE_DIR}
    ${FB_ACTIVEXCORE_SOURCE_DIR}
    ${FB_WEBKITCORE_SOURCE_DIR}
    ${FBLIB_INCLUDE_DIRS}
    ${FB_CONFIG_DIR}
)

file (GLOB GENERAL_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp
    *.h
)

if (WIN32)
    set(platform Win)
elseif (UNIX)
    if (APPLE)
        set(platform Mac)
    else()
        message(FATAL "WebView does not currently support linux.")
    endif()
endif()

set(SOURCES ${GENERAL_SRC})

if (NOT TARGET_ALREADY_EXISTS)

    include(${platform}/platform.cmake)
    set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Libraries")

endif()

append_firebreath_link_library(${PROJECT_NAME})

append_firebreath_include_dir(${CMAKE_CURRENT_SOURCE_DIR})

export_project_dependencies()
