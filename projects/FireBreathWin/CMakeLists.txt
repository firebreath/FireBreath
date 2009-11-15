
# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

include(${CMAKE_DIR}/common.cmake)

set (PROJNAME ${PLUGIN_NAME})

browserplugin_project(${PROJNAME})

add_definitions(
    /D "UNICODE"
    /D "_UNICODE"
    /D "_ATL_STATIC_REGISTRY"
)

# include the other requisite projects
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GECKOSDK_SOURCE_DIR}
    ${ACTIVEXPLUGIN_SOURCE_DIR}
    ${NPAPIPLUGIN_SOURCE_DIR}
    ${WINDOWSCOMMON_SOURCE_DIR}
    ${SCRIPTINGCORE_SOURCE_DIR}
    ${GEN_DIR}/gen
    ${BOOST_SOURCE_DIR}
    )

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp
    *.h
    *.cmake
    )

SET( GENERATED
    ${GEN_DIR}/gen/FireBreathWin.def
    ${GEN_DIR}/gen/firebreathWin.rc
    ${GEN_DIR}/gen/FBControl.htm
    )

SET_SOURCE_FILES_PROPERTIES(
    ${GENERATED}
    PROPERTIES
        GENERATED 1
    )

SOURCE_GROUP(Generated FILES
    ${GENERATED_IDL}
    ${GENERATED}
    ${IDL_FILE}
    )

SET( SOURCES
    ${GENERAL}
    ${GENERATED_IDL}
    ${GENERATED}
    )

add_library(${PROJNAME} SHARED ${SOURCES})

set_target_properties (${PROJNAME} PROPERTIES
    OUTPUT_NAME np${PLUGIN_NAME}
    PROJECT_LABEL ${PROJNAME}
    RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${BIN_DIR}"
    )

target_link_libraries(${PROJNAME}
    ${PLUGIN_PREFIX}_ActiveXPlugin
    ${PLUGIN_PREFIX}_NpapiPlugin
    ScriptingCore
    WindowsCommon
    )

add_dependencies(${PROJNAME}
    ${PLUGIN_PREFIX}_ActiveXPlugin
    ${PLUGIN_PREFIX}_NpapiPlugin
    ScriptingCore
    WindowsCommon
    )
