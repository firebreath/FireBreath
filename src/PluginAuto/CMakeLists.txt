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

Project (${PLUGIN_PREFIX}_PluginAuto)
if (VERBOSE)
    message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")
endif()

include_directories(
    ${FB_GECKOSDK_SOURCE_DIR}
    ${GTK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FB_TEMPLATE_DEST_DIR}
    ${FB_CONFIG_DIR}
    ${Boost_INCLUDE_DIRS}
    ${FB_PLUGINCORE_SOURCE_DIR}
    ${FB_SCRIPTINGCORE_SOURCE_DIR}
    ${FB_NPAPICORE_SOURCE_DIR}
    ${FB_ACTIVEXCORE_SOURCE_DIR}
    ${ATL_INCLUDE_DIRS}
    ${FBLIB_INCLUDE_DIRS}
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
    Mac/np_*.cpp
    )

file (GLOB MAC_INVALIDATINGCOREANIMATION_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac/*ICA.h
    Mac/*ICA.mm
    )
file (GLOB MAC_COREANIMATION_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac/*MacCA.h
    Mac/*MacCA.mm
    )
file (GLOB MAC_COREGRAPHICS_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac/*CG.h
    Mac/*CG.cpp
    )

file (GLOB MAC_QUICKDRAW_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac/*QD.h
    Mac/*QD.cpp
    )

file (GLOB MAC_CARBON_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac/*Carbon.h
    Mac/*Carbon.mm
    Mac/*Carbon.cpp
    )
file (GLOB MAC_COCOA_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac/*Cocoa.h
    Mac/*Cocoa.mm
    Mac/*Cocoa.cpp
    )

if(FBMAC_USE_INVALIDATINGCOREANIMATION)
    set(MAC_SOURCES ${MAC_SOURCES} ${MAC_INVALIDATINGCOREANIMATION_SOURCES})
    set(MAC_SOURCES ${MAC_SOURCES} ${MAC_COREANIMATION_SOURCES})
endif()
if(FBMAC_USE_COREANIMATION)
    set(MAC_SOURCES ${MAC_SOURCES} ${MAC_COREANIMATION_SOURCES})
endif()
if(FBMAC_USE_COREGRAPHICS)
    set(MAC_SOURCES ${MAC_SOURCES} ${MAC_COREGRAPHICS_SOURCES})
endif()
if(FBMAC_USE_QUICKDRAW)
    set(MAC_SOURCES ${MAC_SOURCES} ${MAC_QUICKDRAW_SOURCES})
endif()
if(FBMAC_USE_CARBON)
    set(MAC_SOURCES ${MAC_SOURCES} ${MAC_CARBON_SOURCES})
endif()
if(FBMAC_USE_COCOA)
    set(MAC_SOURCES ${MAC_SOURCES} ${MAC_COCOA_SOURCES})
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
        "${FB_TEMPLATE_DEST_DIR}/FireBreathWin.tlb"
        "${FB_TEMPLATE_DEST_DIR}/FireBreathWin_i.h"
        "${FB_TEMPLATE_DEST_DIR}/FireBreathWin_i.cpp"
        )

    SET (IDL_FILE ${FB_TEMPLATE_DEST_DIR}/FireBreathWin.idl)

    SET_SOURCE_FILES_PROPERTIES(
        ${IDL_FILE}
        "${FB_TEMPLATE_DEST_DIR}/FireBreathWin.tlb"
        "${FB_TEMPLATE_DEST_DIR}/FireBreathWin_i.h"
        "${FB_TEMPLATE_DEST_DIR}/FireBreathWin_p.cpp"
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
            /out "${FB_TEMPLATE_DEST_DIR}" 
            /tlb "FireBreathWin.tlb"
            /h "FireBreathWin_i.h"
            /iid "FireBreathWin_i.cpp"
            /proxy "FireBreathWin_p.cpp"
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
    ${X11_SOURCES})


add_library(${PROJECT_NAME} STATIC ${SOURCES})
ADD_PRECOMPILED_HEADER(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/precompiled_headers.h" "${CMAKE_CURRENT_SOURCE_DIR}/precompiled_headers.cpp" SOURCES)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "FireBreath Generated")
if (WIN32)
    # psapi is needed to get the process info and check if WiX is calling
    # us; wish there was a better way to do this, but there doesn't seem to be
    target_link_libraries(${PROJECT_NAME} psapi)
endif()

if (NOT FB_GUI_DISABLED)
    target_link_libraries(${PROJECT_NAME} ${GUI_LIBRARIES})
endif()
