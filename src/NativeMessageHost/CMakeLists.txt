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
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FB_CONFIG_DIR}
    ${Boost_INCLUDE_DIRS}
    ${FBLIB_INCLUDE_DIRS}
    ${FireWyrm_SOURCE_DIR}
    )

file (GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    [^.]*.h
    [^.]*.cpp
    )

set (SOURCES
    ${SOURCES}
    )

add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "FireWyrmHosts")

target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${LIBDL_LIBRARIES}
    jsoncpp
    )

export(TARGETS ${PROJECT_NAME} APPEND FILE ${FB_EXPORT_FILE})
if (FB_RELEASE)
    install (TARGETS ${PROJECT_NAME}
        EXPORT FireBreath_Exports
        DESTINATION bin/\${BUILD_TYPE}
        COMPONENT FBCORE
        )
endif()
