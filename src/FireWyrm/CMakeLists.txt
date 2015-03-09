#/**********************************************************\ 
#Original Author: Richard Bateman (taxilian)
#
#Created:    Mar 6, 2015
#License:    Dual license model; choose one of two:
#            New BSD License
#            http://www.opensource.org/licenses/bsd-license.php
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#
#Copyright 2015 Gradecam and the FireBreath Development Team
#\**********************************************************/

# Written to work with cmake 2.6
cmake_minimum_required (VERSION 3.0)

Project (FireWyrm)
if (VERBOSE)
    message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")
endif()

if (FIREBREATH)
    add_firebreath_library(jsoncpp)
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FB_CONFIG_DIR}
    ${ScriptingCore_SOURCE_DIR}
    ${PluginCore_SOURCE_DIR}
    ${ATL_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${FBLIB_INCLUDE_DIRS}
    )

file (GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    [^.]*.h
    [^.]*.cpp
    )

set (SOURCES
    ${SOURCES}
    )

add_library(${PROJECT_NAME} STATIC ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "FireBreath Core")

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
