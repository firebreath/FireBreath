

# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

include(${CMAKE_DIR}/common.cmake)
include(${CMAKE_CURRENT_BINARY_DIR}/../PluginConfig.cmake)

set (PROJNAME ${PLUGIN_PREFIX}_NpapiPlugin)
Project (${PROJNAME})
message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

include_directories(
    ${GECKOSDK_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
    ${GEN_DIR}/gen
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

add_library(${PROJNAME} STATIC ${SOURCES})
