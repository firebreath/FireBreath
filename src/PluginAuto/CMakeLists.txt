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

set (PROJNAME ${PLUGIN_PREFIX}_PluginAuto)
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
    ${PLUGINCORE_SOURCE_DIR}
    ${NPAPICORE_SOURCE_DIR}
    ${ATL_INCLUDE_DIRS}
    ${CONFIG_DIR}
    )

file (GLOB COMMON RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    [^.]*.cpp
    [^.]*.h
    [^.]*.txt
    )

set(LogKey "${PLUGIN_NAME}_log4cplus")
if (${LogKey})
    file (GLOB LOGGER RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        log4cplus/*.cpp
        log4cplus/*.h
    )
    SOURCE_GROUP(log4cplus ${LOGGER})
elseif(NOT CUSTOM_LOGGING)
    file (GLOB LOGGER RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        null/*.cpp
        null/*.h
    )
endif()

# X11 *****************************************************************

file (GLOB X11_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    X11/*.h
    X11/*.cpp
    )

SOURCE_GROUP(X11 FILES ${X11_SOURCES})

# Windows *************************************************************

file (GLOB WIN_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Win/*.h
    Win/*.cpp
    )

SOURCE_GROUP(Windows FILES ${WIN_SOURCES})

# Mac *****************************************************************

file (GLOB MAC_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac/*Mac.h
    Mac/*Mac.cpp
    Mac/*Mac.mm
    Mac/*Carbon.h
    Mac/*Carbon.cpp
    )

file (GLOB MAC_QUICKDRAW_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac/*QD.h
    Mac/*QD.cpp
    )

file (GLOB MAC_COCOAWINDOW_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac/*Cocoa.h
    Mac/*Cocoa.mm
    Mac/*Cocoa.cpp
    )

file (GLOB MAC_CGCARBON_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac/*CarbonCG.h
    Mac/*CarbonCG.cpp
    )

file (GLOB MAC_CGCOCOA_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac/*CocoaCG.h
    Mac/*CocoaCG.mm
    )

file (GLOB MAC_COREANIMATION_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac/*CA.h
    Mac/*CA.mm
    )  

if(FBMAC_USE_QUICKDRAW)
    set(MAC_SOURCES ${MAC_SOURCES} ${MAC_QUICKDRAW_SOURCES})
endif()
if(FBMAC_USE_CARBON)
    if(FBMAC_USE_COREGRAPHICS)
        set(MAC_SOURCES ${MAC_SOURCES} ${MAC_CGCARBON_SOURCES})
    endif()
endif()
if(FBMAC_USE_COCOA)
    if(FBMAC_USE_COREGRAPHICS)
        set(MAC_SOURCES ${MAC_SOURCES} ${MAC_CGCOCOA_SOURCES})
    endif()
    set(MAC_SOURCES ${MAC_SOURCES} ${MAC_COCOAWINDOW_SOURCES})
endif()
if(FBMAC_USE_COREANIMATION)
    set(MAC_SOURCES ${MAC_SOURCES} ${MAC_COREANIMATION_SOURCES})
endif()

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

######
# ActiveX Support
######
if (WIN32)
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

    file (GLOB ACTIVEX_OBJECTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        Win/FireBreathWin.*
        Win/ax*.cpp
        Win/ax*.h
    )
SOURCE_GROUP(WindowsATL FILES ${ACTIVEX_OBJECTS})

endif()

SOURCE_GROUP(Mac FILES ${MAC_SOURCES})
set (SOURCES
    ${COMMON}
    ${LOGGER}
    ${WIN_SOURCES}
    ${ACTIVEX_OBJECTS}
    ${GENERATED_IDL}
    ${MAC_SOURCES}
    ${X11_SOURCES}
    )

add_library(${PROJNAME} STATIC ${SOURCES})

