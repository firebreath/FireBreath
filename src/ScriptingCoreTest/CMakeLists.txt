

# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

include(${CMAKE_DIR}/paths.cmake)

Project (UnitTest_ScriptingCore)
message ("Generating project ${PROJECT_NAME}")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
    ${SCRIPTINGCORE_SOURCE_DIR}
    ${FIREBREATH_CONFIG_DIR}
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

add_executable(UnitTest_ScriptingCore ${SOURCES})

add_dependencies(UnitTest_ScriptingCore
    ScriptingCore
    UnitTest++
    )

target_link_libraries (UnitTest_ScriptingCore
    ScriptingCore
    UnitTest++
    )

set_target_properties(UnitTest_ScriptingCore PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}"
)

add_custom_command(TARGET UnitTest_ScriptingCore POST_BUILD
    COMMAND "${BIN_DIR}/${CMAKE_CFG_INTDIR}/UnitTest_ScriptingCore")
