#/**********************************************************\ 
#Original Author: Richard Bateman (taxilian)
#
#Created:    Nov 20, 2009
#License:    Dual license model; choose one of two:
#            New BSD License
#            http://www.opensource.org/licenses/bsd-license.php
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#            
#Copyright 2009 PacketPass, Inc and the Firebreath development team
#\**********************************************************/

# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

Project (FireBreath)

# First, find FB_ROOT
get_filename_component(FB_ROOT ${CMAKE_CURRENT_LIST_FILE} PATH CACHE)

if (FB_CUSTOM_RELEASE)
    set (FB_RELEASE 1)
endif()

if (FB_RELEASE AND NOT FB_CUSTOM_RELEASE)
    # This is needed if you want to make a binary distribution of FireBreath;
    # if you want to do so and need to override the install prefix for some
    # reason you should set FB_CUSTOM_RELEASE instead of FB_RELEASE
    set (CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
endif()

include(${FB_ROOT}/cmake/common.cmake)
set(FB_EXPORT_FILE ${CMAKE_BINARY_DIR}/FireBreath_Exports.cmake)
file(REMOVE ${FB_EXPORT_FILE})
add_subdirectory(${FB_ROOT}/cmake ${FB_BUILD_DIR}/cmake_common)

#Visual Studio Express editions don't want Solution Folders enabled.
if(NOT ATL_LIBRARY) 
    set_property(GLOBAL PROPERTY USE_FOLDERS On)
endif() 

if (APPLE)
    clear_xcode_patches()
    add_definitions(-D__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES=0)
endif()

#check to make sure we're either using WITH_SYSTEM_BOOST or have the firebreath-boost lib installed
fb_check_boost()

if (NOT FB_RELEASE AND FB_PROJECTS_DIR AND NOT EXISTS ${FB_PROJECTS_DIR})
    message(FATAL_ERROR "The specified project directory ${FB_PROJECTS_DIR} is not found")
endif()

if (VERBOSE)
    message("Firebreath solution dir: " ${CMAKE_CURRENT_BINARY_DIR})
endif()

# These are required for FireBreath to run
if (UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
endif()

if (EXISTS ${FB_ROOT}/bin/FireBreath_Exports.cmake AND NOT FB_RELEASE)
    set(FIREBREATH_LIBDIR "${FB_ROOT}/bin")
endif()

if (FIREBREATH_LIBDIR)
    include(${FIREBREATH_LIBDIR}/FireBreath_Exports.cmake)
endif()

add_boost_library(thread)
add_boost_library(system)
if (WITH_SYSTEM_BOOST)
    add_boost_library(date_time)
elseif(NOT FIREBREATH_LIBDIR)
    export(TARGETS ${Boost_LIBRARIES} APPEND FILE ${FB_EXPORT_FILE})
endif()

# Common core projects -- projects that don't have any plugin specific code,
# but are required before any plugins are loaded
# we use an explicit binary directory here in case this file is included out-of-tree
get_target_property(FB_SCRIPTINGCORE_LOADED ScriptingCore TYPE)
if (NOT FB_SCRIPTINGCORE_LOADED)
    add_subdirectory(${FB_SCRIPTINGCORE_SOURCE_DIR} ${FB_SCRIPTINGCORE_BUILD_DIR})
    add_subdirectory(${FB_PLUGINCORE_SOURCE_DIR} ${FB_PLUGINCORE_BUILD_DIR})
    add_subdirectory(${FB_NPAPICORE_SOURCE_DIR} ${FB_NPAPICORE_BUILD_DIR})
    set (FB_EXPORT_TARGETS ScriptingCore PluginCore NpapiCore)
    if (WIN32)
        add_subdirectory(${FB_ACTIVEXCORE_SOURCE_DIR} ${FB_ACTIVEXCORE_BUILD_DIR})
        set (FB_EXPORT_TARGETS ${FB_EXPORT_TARGETS} ActiveXCore)
    endif()
endif()

if (NOT FB_RELEASE)
    message("project dir: ${FB_PROJECTS_DIR}")
    if (EXISTS "${FB_PROJECTS_DIR}/CMakeLists.txt" AND EXISTS "${FB_PROJECTS_DIR}/PluginConfig.cmake")
        message("Single project: ${FB_PROJECTS_DIR}")
        set(CURPROJECT ${FB_PROJECTS_DIR})
        set (FOUND_PROJECTS 1)
        get_filename_component(PROJNAME ${CURPROJECT} NAME_WE)
        if (VERBOSE)
            message("Configuring plugin project ${CURPROJECT} in ${FB_PROJECTS_BINARY_DIR}/${PROJNAME}")
        endif()
        add_firebreath_plugin(${CURPROJECT} ${PROJNAME})
    else()
        # search the project src dir for plugin project directories
        file (GLOB PLUGIN_PROJECTS 
            ${FB_PROJECTS_DIR}/[^.]*
            )

        if (VERBOSE)
            message("Searching ${PROJECTS_SOURCE_DIR}/*: found ${PLUGIN_PROJECTS}")
        endif()

        # loop through the found directories and add any with a CMakeLists.txt and a PluginConfig.cmake
        # as a subproject
        foreach (CURPROJECT ${PLUGIN_PROJECTS})
            if (EXISTS "${CURPROJECT}/CMakeLists.txt" AND EXISTS "${CURPROJECT}/PluginConfig.cmake")
              set (FOUND_PROJECTS 1)
              get_filename_component(PROJNAME ${CURPROJECT} NAME_WE)
              if (VERBOSE)
                  message("Configuring plugin project ${CURPROJECT} in ${FB_PROJECTS_BINARY_DIR}/${PROJNAME}")
              endif()
              add_firebreath_plugin(${CURPROJECT} ${PROJNAME})
            else()
                message("${CURPROJECT}/ does not seem to be a FireBreath Plugin")
            endif()
        endforeach()
    endif()
else()
    message("Projects configured to build FireBreath binaries")
endif()

# Common projects -- projects that don't have any plugin specific code
if (NOT FB_SCRIPTINGCORE_LOADED)
    add_subdirectory(${FB_UNITTEST_FW_SOURCE_DIR} ${FB_UNITTEST_FW_BUILD_DIR})
    #add_subdirectory(${FB_NPAPIHOST_SOURCE_DIR} ${FB_NPAPIHOST_BUILD_DIR})
    add_subdirectory(${FB_SCRIPTINGCORETEST_SOURCE_DIR} ${FB_SCRIPTINGCORETEST_BUILD_DIR})
    if (WIN32)
        add_subdirectory(${FB_ACTIVEXCORETEST_SOURCE_DIR} ${FB_ACTIVEXCORETEST_BUILD_DIR})
    endif()
    #add_subdirectory(${FB_NPAPICORETEST_SOURCE_DIR}) # - not functional, needs to be re-done
endif()

if (NOT FB_RELEASE)
    if (NOT FOUND_PROJECTS AND "${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
        # If this file is the root file then we should have a projects directory
        MESSAGE("No projects found in ${PROJECTS_SOURCE_DIR}/.")
        MESSAGE(FATAL_ERROR "To build examples, run 'prep<ver> examples'")
    endif()
else()
    install(EXPORT FireBreath_Exports DESTINATION "cmake")
endif()



