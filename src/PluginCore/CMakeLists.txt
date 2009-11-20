
# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

include(${CMAKE_DIR}/common.cmake)

set (PROJNAME PluginCore)
Project (${PROJNAME})
message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
    ${SCRIPTINGCORE_SOURCE_DIR}
    ${BOOST_SOURCE_DIR}
    )

file (GLOB EVENTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    PluginEvents/*.h
    PluginEvents/*.cpp
    )

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp
    *.h
    )

source_group(PluginEvents FILES ${EVENTS})

if (WIN32)
    file (GLOB WIN_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        Win/*.h
        Win/*.cpp
        )

    source_group(Windows FILES ${WIN_SOURCES})
endif()

set (SOURCES
    ${GENERAL}
    ${EVENTS}
    ${WIN_SOURCES}
    )

add_library(${PROJNAME} STATIC ${SOURCES})
