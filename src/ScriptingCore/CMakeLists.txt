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

Project (ScriptingCore)
if (VERBOSE)
    message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GTK_INCLUDE_DIRS}
    ${FB_CONFIG_DIR}
    ${FB_PLUGINCORE_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${ATL_INCLUDE_DIRS}
    ${FB_CONFIG_DIR}
    )

file (GLOB DOMTYPES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    DOM.h
    DOM/[^.]*
)

SOURCE_GROUP(DOM FILES ${DOMTYPES})

file (GLOB UTILTYPES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Util/[^.]*
    Shareable*
)

SOURCE_GROUP(Util FILES ${UTILTYPES})

file (GLOB TYPES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    APITypes.*
    variant*
    TypeIDMap.*
    utf8*
    URI*
    SafeQueue*
    JSExceptions.h
    FBPointers.h
    Shareable*
    logging.h
)

file (GLOB JSAPI_OBJECTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    JSAPI*
    BrowserHost.*
    JSObject.*
    JSArray.*
    AsyncBrowser*
    Cross*
    AsyncFunction*
    SyncBrowser*
    *Converter.*
    JSCallback*
    JSFunction*
    JSEvent.*
    )

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Converter*
    Factory*
    Test*
    [^.]*.py
    precompiled_headers.*
    )

SOURCE_GROUP(JSAPI FILES ${JSAPI_OBJECTS})

SOURCE_GROUP(Types FILES ${TYPES})

set (SOURCES
    ${TYPES}
    ${DOMTYPES}
    ${UTILTYPES}
    ${JSAPI_OBJECTS}
    ${GENERAL}
    )

add_library(${PROJECT_NAME} STATIC ${SOURCES})
ADD_PRECOMPILED_HEADER("${PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/precompiled_headers.h" "${CMAKE_CURRENT_SOURCE_DIR}/precompiled_headers.cpp" SOURCES)

target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "FireBreath Core")

export(TARGETS ScriptingCore APPEND FILE ${FB_EXPORT_FILE})
if (FB_RELEASE)
    install (TARGETS ScriptingCore
        EXPORT FireBreath_Exports
        DESTINATION bin/\${BUILD_TYPE}
        COMPONENT FBCORE
        )
endif()

