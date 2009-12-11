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


get_filename_component (CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component (FB_ROOT_DIR "${CMAKE_DIR}/.." ABSOLUTE)
get_filename_component (SOURCE_DIR "${CMAKE_DIR}/../src" ABSOLUTE)
if (BUILD_EXAMPLES)
    get_filename_component (BUILD_DIR "${CMAKE_DIR}/../buildex" ABSOLUTE)
    set(PROJECT_SRC_DIR "examples")
else()
    get_filename_component (BUILD_DIR "${CMAKE_DIR}/../build" ABSOLUTE)
    set(PROJECT_SRC_DIR "projects")
endif()

set (PROJECT_ROOT "${BUILD_DIR}/projects")
set (BIN_DIR "${BUILD_DIR}/bin")

if (NOT EXISTS ${BUILD_DIR})
   file (MAKE_DIRECTORY ${BUILD_DIR})
endif()

# Get the project paths
include(${CMAKE_DIR}/paths.cmake)

# include the build configuration
include(${CMAKE_DIR}/buildconfig.cmake)

if (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/projectConfig.cmake)
    include(${CMAKE_CURRENT_BINARY_DIR}/projectConfig.cmake)
endif()

if (NOT GEN_DIR)
    get_filename_component (GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/../gen" ABSOLUTE)
endif()

function (browserplugin_project PLUGIN_NAME)

    Project (${PLUGIN_NAME})
    message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

    include(${FB_ROOT_DIR}/pluginProjects.cmake)

endfunction(browserplugin_project)


function (include_platform)

    if (WIN32)
        set (PLATFORM_NAME "Win")
    endif()
    include(${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_NAME}/projectDef.cmake)

endfunction(include_platform)
