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

include(${CMAKE_DIR}/common.cmake)
include(${CMAKE_CURRENT_BINARY_DIR}/../PluginConfig.cmake)

set (PROJNAME ${PLUGIN_PREFIX}_PluginCommon)
Project (${PROJNAME})
if (VERBOSE)
    message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")
endif()

include_directories(
    ${GECKOSDK_SOURCE_DIR}
    ${GTK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GEN_DIR}/gen
    ${FIREBREATH_CONFIG_DIR}
    ${Boost_INCLUDE_DIRS}
    )

file (GLOB COMMON RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.h
    *.cpp
    )

get_target_property(log4cplus_target_exists log4cplus TYPE)
if (log4cplus_target_exists)
    file (GLOB LOGGER RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        log4cplus/*.cpp
        log4cplus/*.h
    )
    SOURCE_GROUP(BoostLog ${LOGGER})
elseif(NOT CUSTOM_LOGGING)
    file (GLOB LOGGER RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        null/*.cpp
        null/*.h
    )
endif()

set (SOURCES
    ${COMMON}
    ${LOGGER}
    )

add_library(${PROJNAME} STATIC ${SOURCES})

