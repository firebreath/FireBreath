#/**********************************************************\ 
#Original Author: Richard Bateman (taxilian)
#
#Created:    Nov 20, 2009
#License:    Dual license model; choose one of two:
#            Eclipse Public License - Version 1.0
#            http://www.eclipse.org/legal/epl-v10.html
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#            
#Copyright 2009 PacketPass, Inc and the Firebreath development team
#\**********************************************************/

# Written to work with cmake 2.8
cmake_minimum_required (VERSION 2.8)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.8)

Project (FireBreath)
include(${CMAKE_DIR}/common.cmake)

if (APPLE)
    clear_xcode_patches()
endif()

message("Firebreath solution dir: " ${PROJECT_BINARY_DIR})

# Common projects -- projects that don't have any plugin specific code
add_subdirectory(${UNITTEST_FW_SOURCE_DIR} ${UNITTEST_FW_BINARY_DIR})
add_subdirectory(${NPAPIHOST_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/NpapiHost)
add_subdirectory(${SCRIPTINGCORE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/ScriptingCore)
add_subdirectory(${SCRIPTINGCORETEST_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/ScriptingCoreTest)
add_subdirectory(${PLUGINCORE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/PluginCore)

file (GLOB PLUGIN_PROJECTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SRC_DIR}/*
    )

message("Searching ${PROJECT_SRC_DIR}/*: found ${PLUGIN_PROJECTS}")
foreach (CURPROJECT ${PLUGIN_PROJECTS})
    set (FOUND_PROJECTS 1)
    get_filename_component(PROJNAME ${CURPROJECT} NAME_WE)
    configure_file(${CMAKE_DIR}/projectConfig_main.cmake.in ${PROJECT_ROOT}/${PROJNAME}/projectConfig.cmake COPYONLY)

    message("Configuring plugin project ${CURPROJECT} in ${PROJECT_ROOT}/${PROJNAME}")
    add_subdirectory("${CURPROJECT}" "${PROJECT_ROOT}/${PROJNAME}")
endforeach()

if (NOT FOUND_PROJECTS)
    MESSAGE("No projects found in ${PROJECT_SRC_DIR}/.")
    MESSAGE(FATAL_ERROR "To build examples, run 'prep<ver> examples'")
endif()
