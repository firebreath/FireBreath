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

set(CMAKE_DIR ${CMAKE_SOURCE_DIR}/cmake)

include(${CMAKE_DIR}/common.cmake)

#check to make sure we're either using WITH_SYSTEM_BOOST or have the firebreath-boost lib installed
check_boost()

if (APPLE)
    clear_xcode_patches()
endif()

if (NOT EXISTS ${PROJECTS_SOURCE_DIR})
    message(FATAL_ERROR "The specified project directory ${PROJECTS_SOURCE_DIR} is not found")
endif()

if (VERBOSE)
    message("Firebreath solution dir: " ${PROJECT_BINARY_DIR})
endif()

find_package(Threads REQUIRED)
add_boost_library(thread)
add_boost_library(system)
if (WITH_SYSTEM_BOOST)
    add_boost_library(date_time)
endif()

# Common core projects -- projects that don't have any plugin specific code,
# but are required before any plugins are loaded
add_subdirectory(${SCRIPTINGCORE_SOURCE_DIR})
add_subdirectory(${PLUGINCORE_SOURCE_DIR})
add_subdirectory(${NPAPICORE_SOURCE_DIR})
if (WIN32)
    add_subdirectory(${ACTIVEXCORE_SOURCE_DIR})
endif()

# search the project src dir for plugin project directories
file (GLOB PLUGIN_PROJECTS 
    ${PROJECTS_SOURCE_DIR}/[^.]*
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
      configure_file(${CMAKE_DIR}/projectConfig_main.cmake.in ${PROJECTS_BINARY_DIR}/${PROJNAME}/projectConfig.cmake COPYONLY)
      if (VERBOSE)
          message("Configuring plugin project ${CURPROJECT} in ${PROJECTS_BINARY_DIR}/${PROJNAME}")
      endif()
      add_subdirectory("${CURPROJECT}" "${PROJECTS_BINARY_DIR}/${PROJNAME}")
    else()
        message("${CURPROJECT}/ does not seem to be a FireBreath Plugin")
    endif()
endforeach()

# Common projects -- projects that don't have any plugin specific code
add_subdirectory(${UNITTEST_FW_SOURCE_DIR})
add_subdirectory(${NPAPIHOST_SOURCE_DIR})
add_subdirectory(${SCRIPTINGCORETEST_SOURCE_DIR})
if (WIN32)
    add_subdirectory(${ACTIVEXCORETEST_SOURCE_DIR})
endif()
#add_subdirectory(${NPAPICORETEST_SOURCE_DIR}) # - not functional, needs to be re-done

if (NOT FOUND_PROJECTS)
    MESSAGE("No projects found in ${PROJECTS_SOURCE_DIR}/.")
    MESSAGE(FATAL_ERROR "To build examples, run 'prep<ver> examples'")
endif()
