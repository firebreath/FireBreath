
# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

include(${CMAKE_DIR}/paths.cmake)

Project (ScriptingCore)
message ("Generating project ${PROJECT_NAME}")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
    ${FIREBREATH_CONFIG_DIR}
    ${BOOST_SOURCE_DIR}
    )

file (GLOB TYPES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    APITypes.*
    AutoPtr.*
    variant.*
    vector_list.*
    TypeIDMap.*
)

file (GLOB JSAPI_OBJECTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ./JSAPI*
    BrowserHostWr*
    EventHandlerObject.*
    AsyncBrowser*
    *Converter.*
    )

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Converter*
    Factory*
    Test*
    *.py
    )

SOURCE_GROUP(JSAPI FILES ${JSAPI_OBJECTS})

SOURCE_GROUP(Types FILES ${TYPES})

set (SOURCES
    ${TYPES}
    ${JSAPI_OBJECTS}
    ${GENERAL}
    )

add_library(ScriptingCore STATIC ${SOURCES})
