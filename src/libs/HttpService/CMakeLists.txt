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

project(HttpService)

add_boost_library(thread)
add_boost_library(filesystem)
add_firebreath_library(openssl)
add_firebreath_library(curl)

get_target_property(library_target_exists HttpService TYPE)
if (library_target_exists)
    set(TARGET_ALREADY_EXISTS 1)
else()
    set(TARGET_ALREADY_EXISTS 0)
endif()

include_directories (
    ${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
    ${FB_SCRIPTINGCORE_SOURCE_DIR}
    ${FB_PLUGINAUTO_SOURCE_DIR}
    ${FB_PLUGINCORE_SOURCE_DIR}
    ${FBLIB_INCLUDE_DIRS}
    ${ATL_INCLUDE_DIRS}
    ${FB_CONFIG_DIR}
)

file (GLOB GENERAL_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp
    *.h
)

file (GLOB HTTP_SERVICE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    HTTPService/*.cpp
    HTTPService/*.h
)

file (GLOB COMMON_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    HTTPCommon/*.cpp
    HTTPCommon/*.h
)

file (GLOB CLIENT_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    HTTPClient/*.cpp
    HTTPClient/*.h
)

SOURCE_GROUP(HttpLibs FILES ${HTTP_SOURCE})

if (WIN32)
    set(platform windows)
elseif (UNIX)
    if (APPLE)
        set(platform mac)
    else()
        set(platform linux)
    endif()
endif()

file (GLOB PLATFORM_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Platform/*.h
    Platform/Platform_common.cpp
    Platform/Platform_${platform}.cpp
)

if (WIN32)
    list(APPEND PLATFORM_SRC
        Platform/windows_common.cpp)
endif()

SOURCE_GROUP(Platform FILES ${PLATFORM_SRC})

SOURCE_GROUP(Common FILES ${COMMON_SRC})

SOURCE_GROUP(Client FILES ${CLIENT_SRC})

SOURCE_GROUP(Service FILES ${HTTP_SERVICE})

list(APPEND SOURCES
    ${GENERAL_SRC}
    ${HTTP_SERVICE}
    ${PLATFORM_SRC}
    ${COMMON_SRC}
    ${CLIENT_SRC}
)

if (NOT TARGET_ALREADY_EXISTS)

    add_library (HttpService STATIC ${SOURCES})
    set_target_properties(HttpService PROPERTIES FOLDER "Libraries")

    target_link_libraries(HttpService ${FBLIB_LIBRARIES})

endif()

append_firebreath_link_library(HttpService)

if (WIN32)
    append_firebreath_link_library(Wininet)
    if (ATL_LIBRARY)
        append_firebreath_link_library(${ATL_LIBRARY})
    endif()
elseif (UNIX)
    if (APPLE)
        find_library(CORE_SERVICES CoreServices)
        append_firebreath_link_library(${CORE_SERVICES})
        find_library(SYS_CONFIG SystemConfiguration)
        append_firebreath_link_library(${SYS_CONFIG})
        message("Found: ${CORE_SERVICES} ${SYS_CONFIG}")
    else()
    endif()
endif()

append_firebreath_include_dir(${CMAKE_CURRENT_SOURCE_DIR})

export_project_dependencies()
