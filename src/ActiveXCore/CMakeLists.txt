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

Project (ActiveXCore)
if (VERBOSE)
    message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")
endif()

include_directories(
    ${GECKOSDK_SOURCE_DIR}
    ${GTK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CONFIG_DIR}
    ${FB_SCRIPTINGCORE_SOURCE_DIR}
    ${FB_PLUGINCORE_SOURCE_DIR}
    ${FB_CONFIG_DIR}
    ${ATL_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    )

file (GLOB AXDOM RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    AXDOM/*.cpp
    AXDOM/*.h
)

file (GLOB SCRIPTING RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ActiveXBro*
    ComVar*
    COMJ*
    IDispatchAPI.*
    JSAPI*
)

file (GLOB PLUGIN RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ActiveXFact*
    FBControl.*
    ax*.cpp
    ax*.h
    registrymap.hpp
)

file (GLOB PCH RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	precompiled_*
)

SOURCE_GROUP(ActiveXScripting FILES ${SCRIPTING})

SOURCE_GROUP(ActiveXControl FILES ${PLUGIN})

SOURCE_GROUP(AXDOM FILES ${AXDOM})

set (SOURCES
    ${SCRIPTING}
    ${PLUGIN}
    ${AXDOM}
    ${PCH}
    )

add_library(${PROJECT_NAME} STATIC ${SOURCES})
ADD_PRECOMPILED_HEADER(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/precompiled_headers.h" "${CMAKE_CURRENT_SOURCE_DIR}/precompiled_headers.cpp" SOURCES)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "FireBreath Core")

target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${LIBDL_LIBRARIES}
    PluginCore
    ScriptingCore
)
export(TARGETS ActiveXCore ${CMAKE_THREAD_LIBS_INIT} ${LIBDL_LIBRARIES} APPEND FILE ${FB_EXPORT_FILE})
if (FB_RELEASE)
    install (TARGETS ActiveXCore
        EXPORT FireBreath_Exports
        DESTINATION bin/\${BUILD_TYPE}
        COMPONENT FBCORE
        )
endif()
