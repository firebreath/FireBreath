
# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

include(${CMAKE_DIR}/paths.cmake)

Project (ActiveXPlugin)
message ("Generating project ${PROJECT_NAME}")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
    ${FIREBREATH_CONFIG_DIR}
    )

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
    ./dlldata.c
    ./registrymap.hpp
    ./win_common.h
    ./COM_config.h
    )

file (GLOB IDL_FILE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ./*.idl
    )

set (GENERATED_IDL
    "${FIREBREATH_CONFIG_DIR}/FireBreathWin.tlb"
    "${FIREBREATH_CONFIG_DIR}/FireBreathWin_i.h"
    "${FIREBREATH_CONFIG_DIR}/FireBreathWin_i.c"
    "${FIREBREATH_CONFIG_DIR}/FireBreathWin_p.c"
    )

SET_SOURCE_FILES_PROPERTIES(
    ${IDL_FILE}
    "${FIREBREATH_CONFIG_DIR}/FireBreathWin.tlb"
    "${FIREBREATH_CONFIG_DIR}/FireBreathWin_i.h"
    "${FIREBREATH_CONFIG_DIR}/FireBreathWin_p.c"
    PROPERTIES HEADER_FILE_ONLY 1
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
        /out "${FIREBREATH_CONFIG_DIR}" 
        /tlb "FireBreathWin.tlb"
        /h "FireBreathWin_i.h"
        /iid "FireBreathWin_i.c"
        /proxy "FireBreathWin_p.c"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${IDL_FILE}
    )

set (SOURCES
    ${ACTIVEX_OBJECTS}
    ${JSAPI_OBJECTS}
    ${GENERAL}
    ${GENERATED_IDL}
    )

add_library(ActiveXPlugin STATIC ${SOURCES})
