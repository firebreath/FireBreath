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

set (PROJNAME ${PLUGIN_PREFIX}_ActiveXPlugin)
Project (${PROJNAME})
if (VERBOSE)
    message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")
endif()

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${SCRIPTINGCORE_SOURCE_DIR}
    ${PLUGINCOMMON_SOURCE_DIR}
    ${PLUGINCORE_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${GEN_DIR}/gen
    )

set (GENERATED_IDL
    "${GEN_DIR}/gen/FireBreathWin.tlb"
    "${GEN_DIR}/gen/FireBreathWin_i.h"
    "${GEN_DIR}/gen/FireBreathWin_i.c"
    )

SET (IDL_FILE ${GEN_DIR}/gen/FireBreathWin.idl)

SET_SOURCE_FILES_PROPERTIES(
    ${IDL_FILE}
    "${GEN_DIR}/gen/FireBreathWin.tlb"
    "${GEN_DIR}/gen/FireBreathWin_i.h"
    "${GEN_DIR}/gen/FireBreathWin_p.c"
    PROPERTIES
        HEADER_FILE_ONLY 1
        GENERATED 1
    )

add_custom_command(
    OUTPUT
        ${GENERATED_IDL}
    COMMAND midl.exe ${IDL_FILE}
        /D "_CRT_SECURE_NO_WARNINGS"
        /D "_WINDOWS"
        /D "UNICODE"
        /D "_UNICODE"
        /D "_ATL_STATIC_REGISTRY"
        /D "CMAKE_INTDIR=Debug"
        /nologo /char signed /env win32
        /Oicf 
        /out "${GEN_DIR}/gen" 
        /tlb "FireBreathWin.tlb"
        /h "FireBreathWin_i.h"
        /iid "FireBreathWin_i.c"
        /proxy "FireBreathWin_p.c"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${IDL_FILE}
    )

file (GLOB AXDOM RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ./AXDOM/*
)

source_group(AXDOM FILES ${AXDOM})

file (GLOB ACTIVEX_OBJECTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ./FBControl.*
    ./FireBreathWin.*
)

file (GLOB JSAPI_OBJECTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ./IDispatch*
    ./JSAPI*
    ./ActiveXBrowser*
    ./COMJavascript*
)

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ./axmain.*
    ./axutil.*
    ./axstream*.*
    ./dlldatax.c
    ./registrymap.hpp
    )

set (SOURCES
    ${ACTIVEX_OBJECTS}
    ${JSAPI_OBJECTS}
    ${GENERAL}
    ${GENERATED_IDL}
    ${AXDOM}
    )

add_library(${PROJNAME} STATIC ${SOURCES})
