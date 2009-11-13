

# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

include(${CMAKE_DIR}/paths.cmake)

Project (UnitTest_ActiveXPlugin)
message ("Generating project ${PROJECT_NAME}")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${ACTIVEXPLUGIN_SOURCE_DIR}
    ${SCRIPTINGCORE_SOURCE_DIR}
    ${FIREBREATH_CONFIG_DIR}
    ${UNITTEST_FW_SOURCE_DIR}/src
    )

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ./*.h
    ./*.cpp
    )

set (SOURCES
    ${GENERAL}
    )

add_library(UnitTest_ActiveXPlugin STATIC ${SOURCES})

add_dependencies(UnitTest_ActiveXPlugin
    ActiveXPlugin
    UnitTest++
    )

target_link_libraries (UnitTest_ActiveXPlugin
    ActiveXPlugin
    UnitTest++
    )
