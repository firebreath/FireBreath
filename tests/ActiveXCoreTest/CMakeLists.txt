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

set (PROJNAME UnitTest_ActiveXCore)
Project (${PROJNAME})
if (VERBOSE)
    message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../mock
    ${ACTIVEXCORE_SOURCE_DIR}
    ${SCRIPTINGCORE_SOURCE_DIR}
    ${PLUGINCORE_SOURCE_DIR}
    ${GEN_DIR}/gen
    ${CONFIG_DIR}
    ${UNITTEST_FW_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS}
    ${ATL_INCLUDE_DIRS}
    )

add_definitions(
    /D "UNICODE"
    /D "_UNICODE"
    /D "_ATL_STATIC_REGISTRY"
    /D "FB_UNITTEST"
)

file (GLOB IMPORTED RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${PLUGINAUTO_SOURCE_DIR}/PluginInfo.*
    ../mock/*.cpp
)

file (GLOB TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *Test.h
    )
file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    [^.]*.h
    [^.]*.cpp
    )

source_group(Tests FILES ${TESTS})

source_group(Imported FILES ${IMPORTED})

set (SOURCES
    ${GENERAL}
    ${IMPORTED}
    ${PLUGINAUTO_SOURCE_DIR}/null/NullLogger.cpp
    )

add_executable(${PROJNAME} ${SOURCES})

set_target_properties (${PROJNAME} PROPERTIES
    LINK_FLAGS "${LINK_FLAGS}"
    )

target_link_libraries (${PROJNAME}
    ActiveXCore
    ScriptingCore
    NpapiCore
    PluginCore
    UnitTest++
    )

set_target_properties(${PROJNAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}"
)

add_custom_command(TARGET ${PROJNAME} POST_BUILD
    COMMAND "${BIN_DIR}/${CMAKE_CFG_INTDIR}/${PROJNAME}")
