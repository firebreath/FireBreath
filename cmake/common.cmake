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

get_filename_component (FB_ROOT_DIR "${CMAKE_DIR}/.." ABSOLUTE)
get_filename_component (SOURCE_DIR "${CMAKE_DIR}/../src" ABSOLUTE)

set (BIN_DIR "${CMAKE_BINARY_DIR}/bin")

if (WIN32)
    set (PLATFORM_NAME "Win")
    include(${CMAKE_DIR}/Win.cmake)
    include(${CMAKE_DIR}/wix.cmake)
elseif(APPLE)
    set (PLATFORM_NAME "Mac")
    include(${CMAKE_DIR}/Mac.cmake)
elseif(UNIX)
    set (PLATFORM_NAME "X11")
    include(${CMAKE_DIR}/X11.cmake)
endif()

# include file with build options
include(${CMAKE_DIR}/options.cmake)

# Get the project paths
include(${CMAKE_DIR}/paths.cmake)

# include the build configuration
include(${CMAKE_DIR}/buildconfig.cmake)

if (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/projectConfig.cmake)
    include(${CMAKE_CURRENT_BINARY_DIR}/projectConfig.cmake)
endif()

if (WITH_SYSTEM_BOOST)
    find_package(Boost REQUIRED)
else()
    set(Boost_INCLUDE_DIRS ${BOOST_SOURCE_DIR})
endif()

if (NOT GEN_DIR)
    get_filename_component (GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/../gen" ABSOLUTE)
endif()

function (browserplugin_project PLUGIN_NAME)

    Project (${PLUGIN_NAME})
    message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_NAME}/projectDef.cmake)
        include(${FB_ROOT_DIR}/pluginProjects.cmake)
    endif()

endfunction(browserplugin_project)


function (include_platform)

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_NAME}/projectDef.cmake)
        include(${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_NAME}/projectDef.cmake)
    else()
        message ("Could not find a ${PLATFORM_NAME} directory for the current project")
    endif()

endfunction(include_platform)

