
# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

include(${CMAKE_DIR}/common.cmake)

set (PROJNAME WindowsCommon)
if (WIN32)
    Project (${PROJNAME})
    message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

    add_definitions(
        /D "UNICODE"
        /D "_UNICODE"
        /D "_ATL_STATIC_REGISTRY"
    )

    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${ACTIVEXPLUGIN_SOURCE_DIR}
        ${SCRIPTINGCORE_SOURCE_DIR}
        ${GEN_DIR}/gen
        ${UNITTEST_FW_SOURCE_DIR}/src
        ${BOOST_SOURCE_DIR}
        )

    file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        ./*.h
        ./*.cpp
        )

    set (SOURCES
        ${GENERAL}
        )

    add_library(${PROJNAME} STATIC ${SOURCES})
endif()
