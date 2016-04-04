#/**********************************************************\
#Original Author: Richard Bateman (taxilian)
#
#Created:    Mar 17, 2015
#License:    Dual license model; choose one of two:
#            New BSD License
#            http://www.opensource.org/licenses/bsd-license.php
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#
#Copyright 2015 Gradecam and the FireBreath Development Team
# Special thanks to Kyle Huff for the initial example
#\**********************************************************/

# Written to work with cmake 3.0
cmake_minimum_required (VERSION 3.0)

Project (FireWyrmNativeMessageHost)
if (VERBOSE)
    message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")
endif()

if (FIREBREATH)
    include(${FB_ROOT}/cmake/paths.cmake)
    add_firebreath_library(jsoncpp)
endif()

include_directories(
    ${FB_GECKOSDK_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FB_CONFIG_DIR}
    ${Boost_INCLUDE_DIRS}
    ${FBLIB_INCLUDE_DIRS}
    ${FireWyrm_SOURCE_DIR}
    ${FB_GECKOSDK_SOURCE_DIR}
    ${GTK_INCLUDE_DIRS}
    )

file (GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    [^.]*.h
    [^.]*.cpp
    )

file (GLOB X11_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    X11/[^.]*.h
    X11/[^.]*.cpp
    )

SOURCE_GROUP(X11 FILES ${X11_SOURCES})

file (GLOB WIN_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Win/[^.]*.h
    Win/[^.]*.cpp
    )

SOURCE_GROUP(Windows FILES ${WIN_SOURCES})

file (GLOB MAC_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac/[^.]*.h
    Mac/[^.]*.cpp
    Mac/[^.]*.mm
    )

SOURCE_GROUP(Mac FILES ${MAC_SOURCES})

if (WIN32)
    set_source_files_properties(${MAC_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
    set_source_files_properties(${X11_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
elseif(APPLE)
    set_source_files_properties(${WIN_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
    set_source_files_properties(${X11_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
elseif(UNIX)
    set_source_files_properties(${WIN_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
    set_source_files_properties(${MAC_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
endif()

set (SOURCES
    ${SOURCES}
    ${WIN_SOURCES}
    ${MAC_SOURCES}
    ${X11_SOURCES}
    )

add_executable(${PROJECT_NAME} ${SOURCES})

if (NOT FBSTRING_HostFileName)
    set(FBSTRING_HostFileName ${PROJECT_NAME})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES 
    OUTPUT_NAME ${FBSTRING_HostFileName}
    FOLDER "FireWyrmHosts"
    )

if (FIREBREATH)
    link_boost_library(${PROJECT_NAME} filesystem)
endif()

if (UNIX AND NOT CMAKE_SYSTEM_NAME MATCHES "BSD")
    set(LIBDL_LIBRARIES dl)
endif()

target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${LIBDL_LIBRARIES}
    jsoncpp
    ${GTK_LIBRARIES}
    )

export(TARGETS ${PROJECT_NAME} APPEND FILE ${FB_EXPORT_FILE})
if (FB_RELEASE)
    install (TARGETS ${PROJECT_NAME}
        EXPORT FireBreath_Exports
        DESTINATION bin/\${BUILD_TYPE}
        COMPONENT FBCORE
        )
endif()
