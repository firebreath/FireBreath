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

# First, find FB_ROOT
get_filename_component(FB_ROOT ${CMAKE_CURRENT_LIST_FILE} PATH)

Project (FireBreath)

option(FB_AUTO_GIT "Enable automatic git operations." ON)

include(${FB_ROOT}/cmake/common.cmake)

set(FB_EXPORT_FILE ${CMAKE_BINARY_DIR}/FireBreath_Exports.cmake)
file(REMOVE ${FB_EXPORT_FILE})
add_subdirectory(${FB_ROOT}/cmake ${CMAKE_CURRENT_BINARY_DIR}/cmake_common)

if (APPLE)
    clear_xcode_patches()
    add_definitions(-D__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES=0)
endif()

#check to make sure we're either using WITH_SYSTEM_BOOST or have the firebreath-boost lib installed
fb_check_boost()

if (UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
endif()

if (WITH_SYSTEM_BOOST)
    if ( APPLE )
        SET(Boost_USE_STATIC_LIBS ON)
    elseif (WIN32)
        SET(Boost_USE_STATIC_LIBS ON)
        if (WITH_DYNAMIC_MSVC_RUNTIME)
            SET(Boost_USE_STATIC_RUNTIME OFF)
        else (WITH_DYNAMIC_MSVC_RUNTIME)
            SET(Boost_USE_STATIC_RUNTIME ON)
        endif (WITH_DYNAMIC_MSVC_RUNTIME)
    endif ()
endif()

add_boost_library(thread)
add_boost_library(system)

if (UNIX AND NOT APPLE) 
    add_boost_library(regex)
endif()

if (Boost_MAJOR_VERSION GREATER 0 AND Boost_MINOR_VERSION GREATER 47)
    add_boost_library(chrono)
endif ()
#if(NOT WITH_SYSTEM_BOOST AND NOT FIREBREATH_LIBDIR)
    #export(TARGETS ${Boost_LIBRARIES} APPEND FILE ${FB_EXPORT_FILE})
#endif()

# Common core projects -- projects that don't have any plugin specific code,
# but are required before any plugins are loaded
# we use an explicit binary directory here in case this file is included out-of-tree
add_subdirectory(${FB_ROOT}/src/ScriptingCore ${CMAKE_CURRENT_BINARY_DIR}/ScriptingCore)
add_subdirectory(${FB_ROOT}/src/PluginCore ${CMAKE_CURRENT_BINARY_DIR}/PluginCore)
add_subdirectory(${FB_ROOT}/src/NpapiCore ${CMAKE_CURRENT_BINARY_DIR}/NpapiCore)
add_subdirectory(${FB_ROOT}/src/FireWyrm ${CMAKE_CURRENT_BINARY_DIR}/FireWyrm)
#set (FB_EXPORT_TARGETS ScriptingCore PluginCore NpapiCore)
if (WIN32)
    add_subdirectory(${FB_ROOT}/src/ActiveXCore ${CMAKE_CURRENT_BINARY_DIR}/ActiveXCore)
    #set (FB_EXPORT_TARGETS ${FB_EXPORT_TARGETS} ActiveXCore)
endif()

#add_subdirectory(${FB_ROOT}/tests/unittest-cpp/UnitTest++ ${CMAKE_CURRENT_BINARY_DIR}/unittest++)
#add_subdirectory(${FB_NPAPIHOST_SOURCE_DIR} ${FB_NPAPIHOST_BUILD_DIR})
#add_subdirectory(${FB_ROOT}/tests/ScriptingCoreTest ${CMAKE_CURRENT_BINARY_DIR}/ScriptingCoreTest)
#if (WIN32)
    #add_subdirectory(${FB_ROOT}/tests/ActiveXCoreTest ${CMAKE_CURRENT_BINARY_DIR}/ActiveXCoreTest)
#endif()
#add_subdirectory(${FB_NPAPICORETEST_SOURCE_DIR}) # - not functional, needs to be re-done

add_subdirectory(${FB_ROOT}/src/NativeMessageHost ${CMAKE_CURRENT_BINARY_DIR}/NativeMessageHost)

#install(EXPORT FireBreath_Exports DESTINATION "cmake")

message("Boost includes: ${Boost_INCLUDE_DIRS}")

export_project_dependencies()
