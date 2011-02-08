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
get_filename_component (TEST_DIR "${CMAKE_DIR}/../tests" ABSOLUTE)

set (BIN_DIR "${CMAKE_BINARY_DIR}/bin")
set (FIREBREATH YES INTERNAL)

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

# include file with the crazy configure_template function
include(${CMAKE_DIR}/configure_template.cmake)

# include file with build options
include(${CMAKE_DIR}/options.cmake)

# Get the project paths
include(${CMAKE_DIR}/paths.cmake)

# include the build configuration
include(${CMAKE_DIR}/buildconfig.cmake)

if (NOT GEN_DIR)
    get_filename_component (GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/../gen" ABSOLUTE)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/PluginConfig.cmake)
    include(${CMAKE_DIR}/PluginConfigDefaults.cmake)
    include (${CMAKE_CURRENT_SOURCE_DIR}/PluginConfig.cmake)
endif()

macro (browserplugin_project PLUGIN_NAME)

    Project (${PLUGIN_NAME})
    message ("Generating project ${PROJECT_NAME} in ${CMAKE_CURRENT_BINARY_DIR}")

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_NAME}/projectDef.cmake)
        set(CMAKE_CURRENT_PLUGIN_DIR ${CMAKE_CURRENT_SOURCE_DIR})
        include(${CMAKE_CURRENT_BINARY_DIR}/projectConfig.cmake)
        include(${FB_ROOT_DIR}/pluginProjects.cmake)
    endif()

endmacro(browserplugin_project)

macro (include_platform)

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_NAME}/projectDef.cmake)
        include(${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_NAME}/projectDef.cmake)
    else()
        message ("Could not find a ${PLATFORM_NAME} directory for the current project")
    endif()

endmacro(include_platform)

macro (link_boost_library PROJECT BOOST_LIB)
    add_boost_library(${BOOST_LIB})
    if (NOT WITH_SYSTEM_BOOST)
        target_link_libraries(${PROJECT} boost_${BOOST_LIB})
    endif()
endmacro(link_boost_library)

macro (add_boost_library BOOST_LIB)

    if (WITH_SYSTEM_BOOST)
        find_package(Boost COMPONENTS ${BOOST_LIB} REQUIRED)
    else()
        list(APPEND Boost_INCLUDE_DIRS ${BOOST_SOURCE_DIR})
        list(REMOVE_DUPLICATES Boost_INCLUDE_DIRS)
    
        list(APPEND Boost_LIBRARIES boost_${BOOST_LIB})
        list(REMOVE_DUPLICATES Boost_LIBRARIES)
        if (NOT TARGET boost_${BOOST_LIB})
            add_subdirectory(${BOOST_SOURCE_DIR}/libs/${BOOST_LIB} ${CMAKE_BINARY_DIR}/boost/libs/${BOOST_LIB})
        endif()
    endif()

endmacro (add_boost_library)

macro (add_firebreath_library_dir dir)
    list(APPEND FBLIB_DIRS ${dir})
    list(REMOVE_DUPLICATES FBLIB_DIRS)
endmacro()

macro (add_firebreath_library project_name)

    foreach(LIB_DIR ${FBLIB_DIRS})
        if (EXISTS ${LIB_DIR}/${project_name}/CMakeLists.txt)
            get_target_property(library_target_exists ${project_name} TYPE)
            if (NOT library_target_exists)
                set(FBLIB_DEFINITIONS)
                add_subdirectory(${LIB_DIR}/${project_name} ${CMAKE_CURRENT_BINARY_DIR}/fblibs/${project_name})
                if (FBLIB_DEFINITIONS)
                    add_definitions(${FBLIB_DEFINITIONS})
                endif()
            endif()
        endif()
    endforeach()
    set (LIB_KEY ${PLUGIN_NAME}_${project_name})
    set (${LIB_KEY} YES)
    set (${LIB_KEY} YES PARENT_SCOPE)
    message("Set ${LIB_KEY} to ${${LIB_KEY}}")

endmacro(add_firebreath_library)

macro (append_firebreath_link_library name)
    list(APPEND FBLIB_LIBRARIES ${name})
    list(REMOVE_DUPLICATES FBLIB_LIBRARIES)
endmacro(append_firebreath_link_library)

macro (append_firebreath_include_dir dir)
    list(APPEND FBLIB_INCLUDE_DIRS ${dir})
    list(REMOVE_DUPLICATES FBLIB_INCLUDE_DIRS)
endmacro(append_firebreath_include_dir)

macro (add_firebreath_definition DEFS)
    list(APPEND FBLIB_DEFINITIONS ${DEFS})
    add_definitions(${DEFS})
endmacro()

macro (export_project_dependencies)
    if (FBLIB_LIBRARIES)
        list(REMOVE_DUPLICATES FBLIB_LIBRARIES)
    endif()
    if (FBLIB_INCLUDE_DIRS)
        list(REMOVE_DUPLICATES FBLIB_INCLUDE_DIRS)
    endif()
    set (FBLIB_LIBRARIES ${FBLIB_LIBRARIES} PARENT_SCOPE)
    set (FBLIB_INCLUDE_DIRS ${FBLIB_INCLUDE_DIRS} PARENT_SCOPE)
    set (FBLIB_DEFINITIONS ${FBLIB_DEFINITIONS} PARENT_SCOPE)
    set (Boost_LIBRARIES ${Boost_LIBRARIES} PARENT_SCOPE)
    set (Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} PARENT_SCOPE)
endmacro (export_project_dependencies)

function (check_boost)
    if (NOT WITH_SYSTEM_BOOST)
        if (NOT EXISTS ${BOOST_SOURCE_DIR})
            file(MAKE_DIRECTORY ${BOOST_SOURCE_DIR})
        endif()
        if (NOT EXISTS ${BOOST_SOURCE_DIR}/boost/)
            set (FB_URL "https://github.com/firebreath/firebreath-boost/tarball/master")
            message("Boost not found; downloading latest FireBreath-boost from GitHub (http://github.com/firebreath/firebreath-boost)")
            find_program(GIT git NO_DEFAULT_PATHS)
            if (EXISTS ${FB_ROOT_DIR}/.git AND NOT ${GIT} MATCHES "GIT-NOTFOUND")
                message("Using git")
                if (${GIT} MATCHES "GIT-NOTFOUND")
                    message(FATAL_ERROR "Can't find git in the path. Please run 'git submodule update --init --recursive' to get firebreath-boost.")
                endif()
                execute_process(
                    COMMAND ${GIT}
                    submodule update --recursive --init
                    WORKING_DIRECTORY "${FB_ROOT_DIR}")
            else()
                message("Downloading...")
                find_program(CURL curl)
                find_program(WGET wget PATHS "${CMAKE_DIR}/")
                if (NOT ${WGET} MATCHES "WGET-NOTFOUND")
                    execute_process(
                        COMMAND ${WGET}
                        --no-check-certificate
                        -O "${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz"
                        "${FB_URL}"
                        )
                elseif (NOT ${CURL} MATCHES "CURL-NOTFOUND")
                    execute_process(
                        COMMAND ${CURL}
                        -L "${FB_URL}"
                        -k
                        OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz"
                        )
                else()
                    file (DOWNLOAD "${FB_URL}" "${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz" STATUS DL_STATUS SHOW_PROGRESS)
                    message("Result: ${DL_STATUS}")
                endif()
                if (NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz")
                    message("Error downloading firebreath-boost. Please get firebreath-boost from ")
                    message("http://github.com/firebreath/firebreath-boost and install it into")
                    message("the src/3rdParty/boost directory.  When properly installed, you should have:")
                    message("    src/3rdParty/boost/boost/")
                    message("    src/3rdParty/boost/libs/")
                    message(FATAL_ERROR "firebreath-boost could not be installed. Please install manually.")
                endif()
                find_program(TAR tar NO_DEFAULT_PATHS)
                find_program(GZIP gzip NO_DEFAULT_PATHS)
                if (WIN32)
                    message("Using 7-zip to extract the archive")
                    find_program(SEVZIP 7za PATHS "${CMAKE_DIR}/")
                    execute_process(
                        COMMAND ${SEVZIP}
                        e "${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz"
                        WORKING_DIRECTORY "${BOOST_SOURCE_DIR}"
                        OUTPUT_QUIET
                        )
                    file (GLOB TAR_FILE 
                        ${BOOST_SOURCE_DIR}/*.tar
                        )
                    execute_process(
                        COMMAND ${SEVZIP}
                        x "${TAR_FILE}"
                        WORKING_DIRECTORY "${BOOST_SOURCE_DIR}"
                        OUTPUT_QUIET
                        )
                    file(REMOVE ${TAR_FILE})
                elseif (NOT ${TAR} MATCHES "TAR-NOTFOUND" AND NOT ${GZIP} MATCHES "GZIP-NOTFOUND")
                    message("Using tar xzf to extract the archive")
                    execute_process(
                        COMMAND ${TAR}
                        xzf "${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz"
                        WORKING_DIRECTORY "${BOOST_SOURCE_DIR}"
                        OUTPUT_QUIET
                        )
                else()
                    message("Please extract ${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz and ")
                    message("move the boost/ and libs/ subdirectories ")
                    message("to ${BOOST_SOURCE_DIR}/boost and ${BOOST_SOURCE_DIR}/libs")
                    message(FATAL_ERROR "Firebreath-boost not installed! (follow above directions to resolve)")
                endif()
                #file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz)
                message("Installing firebreath-boost...")
                file (GLOB _BOOST_FILES
                    ${BOOST_SOURCE_DIR}/firebreath-firebreath-boost*/*)
                foreach (_CUR_FILE ${_BOOST_FILES})
                    get_filename_component (_CUR_FILENAME ${_CUR_FILE} NAME)
                    file(RENAME ${_CUR_FILE} ${BOOST_SOURCE_DIR}/${_CUR_FILENAME})
                endforeach()
                message("Cleaning up firebreath-boost temp install files...")
                file (GLOB _BOOST_TMPFILES
                    ${BOOST_SOURCE_DIR}/firebreath*
                    ${BOOST_SOURCE_DIR}/pax*)
                file (REMOVE_RECURSE ${_BOOST_TMPFILES})
            endif()
        endif()
    endif()
endfunction()
