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

# Previously we included this file all over the place; we should never
# do that anymore.
if (NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${FB_ROOT}")
    message("!!! WARNING! You generally should not include common.cmake from your plugin project!")
endif()

get_filename_component (FB_ROOT_DIR "${FB_ROOT}/cmake/.." ABSOLUTE CACHE)
get_filename_component (FB_SOURCE_DIR "${FB_ROOT}/cmake/../src" ABSOLUTE CACHE)
get_filename_component (FB_TEST_DIR "${FB_ROOT}/cmake/../tests" ABSOLUTE CACHE)

set (FB_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set (FB_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin")
set (FIREBREATH YES)

if (WIN32)
    set (FB_PLATFORM_NAME "Win")
    include(${FB_ROOT}/cmake/Win.cmake)
    include(${FB_ROOT}/cmake/wix.cmake)
elseif(APPLE)
    set (FB_PLATFORM_NAME "Mac")
    include(${FB_ROOT}/cmake/Mac.cmake)
elseif(UNIX)
    set (FB_PLATFORM_NAME "X11")
    include(${FB_ROOT}/cmake/X11.cmake)
endif()

# include file with the crazy configure_template function
include(${FB_ROOT}/cmake/configure_template.cmake)

# include file with build options
include(${FB_ROOT}/cmake/options.cmake)

# Get the project paths
include(${FB_ROOT}/cmake/paths.cmake)

# include the build configuration defaults
include(${FB_ROOT}/cmake/buildconfig.cmake)

function (add_firebreath_plugin PROJECT_PATH PROJECT_NAME)
    # This is a function so that the includes won't contaminate the parent
    set (FB_CURRENT_PLUGIN_DIR "${PROJECT_PATH}")
    include(${FB_ROOT}/cmake/PluginConfigDefaults.cmake)
    include(${PROJECT_PATH}/PluginConfig.cmake)
    include(${FB_ROOT}/cmake/CommonPluginConfig.cmake)
    include(${FB_ROOT}/cmake/pluginProjects.cmake)
    add_subdirectory(${PROJECT_PATH} ${FB_PROJECTS_BINARY_DIR}/${PLUGIN_NAME})
endfunction(add_firebreath_plugin)

macro (browserplugin_project PLUGIN_NAME)

    message("browserplugin_project is deprecated; please use Project(${PLUGIN_NAME}) instead")
    Project (${PLUGIN_NAME})

endmacro(browserplugin_project)

macro (include_platform)

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${FB_PLATFORM_NAME}/projectDef.cmake)
        include(${CMAKE_CURRENT_SOURCE_DIR}/${FB_PLATFORM_NAME}/projectDef.cmake)
    else()
        message ("Could not find a ${FB_PLATFORM_NAME} directory for the current project")
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
        list(APPEND Boost_INCLUDE_DIRS ${FB_BOOST_SOURCE_DIR})
        list(REMOVE_DUPLICATES Boost_INCLUDE_DIRS)
    
        list(APPEND Boost_LIBRARIES boost_${BOOST_LIB})
        list(REMOVE_DUPLICATES Boost_LIBRARIES)
        get_target_property(_BL_EXISTS boost_${BOOST_LIB} TYPE)
        if (NOT _BL_EXISTS)
            add_subdirectory(${FB_BOOST_SOURCE_DIR}/libs/${BOOST_LIB} ${CMAKE_BINARY_DIR}/boost/libs/${BOOST_LIB})
        endif()
    endif()

endmacro (add_boost_library)

macro (add_firebreath_library_dir dir)
    list(APPEND FBLIB_DIRS ${dir})
    list(REMOVE_DUPLICATES FBLIB_DIRS)
endmacro()

macro (add_firebreath_library project_name)

    set (_FOUND_LIB 0)
    foreach(_LIB_DIR ${FBLIB_DIRS})
        if (NOT _FOUND_LIB AND EXISTS ${_LIB_DIR}/${project_name}/CMakeLists.txt)
            get_target_property(library_target_exists log4cplus TYPE)
            if (library_target_exists)
                set (_CUR_BINDIR ${CMAKE_CURRENT_BINARY_DIR}/fblibs/${PLUGIN_NAME}/${project_name})
            else()
                set (_CUR_BINDIR ${CMAKE_CURRENT_BINARY_DIR}/fblibs/${project_name})
            endif()
            set(FBLIB_DEFINITIONS)
            add_subdirectory(${_LIB_DIR}/${project_name} ${_CUR_BINDIR})
            if (FBLIB_DEFINITIONS)
                add_definitions(${FBLIB_DEFINITIONS})
            endif()
            set (_FOUND_LIB 1)
        endif()
    endforeach()
    if (NOT _FOUND_LIB)
        message(FATAL "Could not find FireBreath Library ${project_name}")
    endif()
    set (_LIB_KEY ${PLUGIN_NAME}_${project_name})
    set (${_LIB_KEY} YES)
    set (${_LIB_KEY} YES PARENT_SCOPE)

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

# Warn if the http_proxy is set but not the https_proxy
# Check for both uppercase (windows) and lowercase (linux) variants
# CMake can't use the ENV function directly in conditional statements (so create temporary local variables to do the check)
# http://marc.info/?l=cmake&m=119419263535736&w=2
function(proxy_setup)
    SET( HTTP_PROXY $ENV{HTTP_PROXY} )
    SET( HTTPS_PROXY $ENV{HTTPS_PROXY} )
    SET( http_proxy $ENV{http_proxy} )
    SET( https_proxy $ENV{https_proxy} )
    if(HTTP_PROXY AND NOT HTTPS_PROXY)
        message("!!!! WARNING: HTTP_PROXY env var set, but we need HTTPS_PROXY. Attempting to use HTTP_PROXY FOR HTTPS_PROXY")
        set(ENV{HTTPS_PROXY} ${HTTP_PROXY})
    endif()
    
    if(http_proxy AND NOT https_proxy)
        message("!!!! WARNING: http_proxy env var set, but we need https_proxy. Attempting to use http_proxy FOR https_proxy")
        set(ENV{https_proxy} ${http_proxy})
    endif()
    #message($ENV{https_proxy})
    #message($ENV{HTTPS_PROXY})
endfunction(proxy_setup)

function (fb_check_boost)
    if (NOT WITH_SYSTEM_BOOST)
        if (NOT EXISTS ${FB_BOOST_SOURCE_DIR})
            file(MAKE_DIRECTORY ${FB_BOOST_SOURCE_DIR})
        endif()
        if (NOT EXISTS ${FB_BOOST_SOURCE_DIR}/boost/)
            set (FB_URL "https://github.com/firebreath/firebreath-boost/tarball/master")
            message("Boost not found; downloading latest FireBreath-boost from GitHub (http://github.com/firebreath/firebreath-boost)")
            find_program(GIT git
                PATHS
                )
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
                find_program(WGET wget PATHS "${FB_ROOT}/cmake/")
                proxy_setup()
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
                    find_program(SEVZIP 7za PATHS "${FB_ROOT}/cmake/")
                    execute_process(
                        COMMAND ${SEVZIP}
                        e "${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz"
                        WORKING_DIRECTORY "${FB_BOOST_SOURCE_DIR}"
                        OUTPUT_QUIET
                        )
                    file (GLOB TAR_FILE 
                        ${FB_BOOST_SOURCE_DIR}/*.tar
                        )
                    execute_process(
                        COMMAND ${SEVZIP}
                        x "${TAR_FILE}"
                        WORKING_DIRECTORY "${FB_BOOST_SOURCE_DIR}"
                        OUTPUT_QUIET
                        )
                    file(REMOVE ${TAR_FILE})
                elseif (NOT ${TAR} MATCHES "TAR-NOTFOUND" AND NOT ${GZIP} MATCHES "GZIP-NOTFOUND")
                    message("Using tar xzf to extract the archive")
                    execute_process(
                        COMMAND ${TAR}
                        xzf "${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz"
                        WORKING_DIRECTORY "${FB_BOOST_SOURCE_DIR}"
                        OUTPUT_QUIET
                        )
                else()
                    message("Please extract ${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz and ")
                    message("move the boost/ and libs/ subdirectories ")
                    message("to ${FB_BOOST_SOURCE_DIR}/boost and ${FB_BOOST_SOURCE_DIR}/libs")
                    message(FATAL_ERROR "Firebreath-boost not installed! (follow above directions to resolve)")
                endif()
                #file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz)
                message("Installing firebreath-boost...")
                file (GLOB _BOOST_FILES
                    ${FB_BOOST_SOURCE_DIR}/firebreath-firebreath-boost*/*)
                foreach (_CUR_FILE ${_BOOST_FILES})
                    get_filename_component (_CUR_FILENAME ${_CUR_FILE} NAME)
                    file(RENAME ${_CUR_FILE} ${FB_BOOST_SOURCE_DIR}/${_CUR_FILENAME})
                endforeach()
                message("Cleaning up firebreath-boost temp install files...")
                file (GLOB _BOOST_TMPFILES
                    ${FB_BOOST_SOURCE_DIR}/firebreath*
                    ${FB_BOOST_SOURCE_DIR}/pax*)
                file (REMOVE_RECURSE ${_BOOST_TMPFILES})
            endif()
        endif()
    endif()
endfunction()

MACRO(ADD_PRECOMPILED_HEADER PROJECT_NAME PrecompiledHeader PrecompiledSource SourcesVar)
    IF(MSVC)
        GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
        SET(__PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${PrecompiledBasename}.pch")

        # Found this example of setting up PCH in the cmake source code under Tests/PrecompiledHeader
        SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
            COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${__PrecompiledBinary}\"")

        SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
            PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledBasename}.h\"")
    elseif (APPLE)
        message("Setting precompiled header ${PrecompiledHeader} on ${PROJECT_NAME}")
        SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER YES)
        SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_PREFIX_HEADER "${PrecompiledHeader}")
    endif()
ENDMACRO(ADD_PRECOMPILED_HEADER)
