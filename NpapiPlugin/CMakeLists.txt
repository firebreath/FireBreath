

# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

include(${CMAKE_DIR}/paths.cmake)

Project (NpapiPlugin)
message ("Generating project ${PROJECT_NAME}")

include_directories(
    ${GECKOSDK_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
    ${SCRIPTINGCORE_SOURCE_DIR}
    ${FIREBREATH_CONFIG_DIR}
    ${BOOST_SOURCE_DIR}
    )

file (GLOB SCRIPTING RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    NPJava*
    NPObject*
    NpapiBrowser*
)

file (GLOB PLUGIN RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    NpapiPlugin*.cpp
    NpapiPlugin*.h
    NpapiTypes.*
    )

SOURCE_GROUP(NpapiScripting FILES ${SCRIPTING})

SOURCE_GROUP(NpapiPlugin FILES ${PLUGIN})

set (SOURCES
    ${SCRIPTING}
    ${PLUGIN}
    )

add_library(NpapiPlugin STATIC ${SOURCES})
