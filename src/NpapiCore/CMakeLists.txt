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

set (PROJNAME NpapiCore)
Project (${PROJNAME})
if (VERBOSE)
    message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")
endif()

include_directories(
    ${GECKOSDK_SOURCE_DIR}
    ${GTK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CONFIG_DIR}
    ${SCRIPTINGCORE_SOURCE_DIR}
    ${PLUGINCORE_SOURCE_DIR}
    ${FIREBREATH_CONFIG_DIR}
    ${ATL_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    )

file (GLOB SCRIPTING RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    NPJava*
    NPObject*
    NpapiBrowser*
    NPVariantUtil.h
)

file (GLOB PLUGIN RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    NpapiPlugin*.cpp
    NpapiPlugin*.h
    NpapiStream.*
    NpapiTypes.*
    NpapiFactory*
    )

SOURCE_GROUP(NpapiScripting FILES ${SCRIPTING})

SOURCE_GROUP(NpapiPlugin FILES ${PLUGIN})

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
endif()
if(APPLE)
    set_source_files_properties(${WIN_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
    set_source_files_properties(${X11_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
elseif(UNIX)
    set_source_files_properties(${WIN_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
    set_source_files_properties(${MAC_SOURCES} PROPERTIES HEADER_FILE_ONLY 1)
endif()

set (SOURCES
    ${SCRIPTING}
    ${PLUGIN}
    ${WIN_SOURCES}
    ${MAC_SOURCES}
    ${X11_SOURCES}
    )

add_library(${PROJNAME} STATIC ${SOURCES})

if (UNIX)
    # Link with libdl on Mac and X11
    set(LIBDL_LIBRARIES dl)
endif()
target_link_libraries(${PROJNAME}
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${LIBDL_LIBRARIES}
    )
