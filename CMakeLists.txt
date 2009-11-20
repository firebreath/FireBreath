
# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

include(${CMAKE_DIR}/common.cmake)

Project (FireBreath)
message("Firebreath solution dir: " ${PROJECT_BINARY_DIR})

# Common projects -- projects that don't have any plugin specific code
add_subdirectory(${UNITTEST_FW_SOURCE_DIR} ${UNITTEST_FW_BINARY_DIR})
add_subdirectory(${NPAPIHOST_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/NpapiHost)
add_subdirectory(${SCRIPTINGCORE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/ScriptingCore)
add_subdirectory(${SCRIPTINGCORETEST_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/ScriptingCoreTest)
add_subdirectory(${PLUGINCORE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/PluginCore)

file (GLOB PLUGIN_PROJECTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    projects/*
    )

foreach (CURPROJECT ${PLUGIN_PROJECTS})
    get_filename_component(PROJNAME ${CURPROJECT} NAME_WE)
    configure_file(${CMAKE_DIR}/projectConfig_main.cmake.in ${PROJECT_ROOT}/${PROJNAME}/projectConfig.cmake COPYONLY)

    message("Configuring plugin project ${CURPROJECT} in ${PROJECT_ROOT}/${PROJNAME}")
    add_subdirectory("${CURPROJECT}" "${PROJECT_ROOT}/${PROJNAME}")
endforeach()
