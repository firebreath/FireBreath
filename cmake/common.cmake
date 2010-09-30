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

set(Boost_INCLUDE_DIRS ${BOOST_SOURCE_DIR})

if (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/projectConfig.cmake)
    include(${CMAKE_CURRENT_BINARY_DIR}/projectConfig.cmake)
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

function (link_boost_library PROJECT BOOST_LIB)
    add_boost_library(${BOOST_LIB})
    if (NOT WITH_SYSTEM_BOOST)
        target_link_libraries(${PROJECT} boost_${BOOST_LIB})
    endif()
endfunction(link_boost_library)

function (add_boost_library BOOST_LIB)

    if (WITH_SYSTEM_BOOST)
        find_package(Boost COMPONENTS thread REQUIRED)
    else()
        list(APPEND Boost_LIBRARIES boost_thread)
        if (NOT TARGET boost_${BOOST_LIB})
            add_subdirectory(${BOOST_SOURCE_DIR}/libs/${BOOST_LIB} ${CMAKE_BINARY_DIR}/boost/libs/${BOOST_LIB})
        endif()
    endif()

endfunction(add_boost_library)
