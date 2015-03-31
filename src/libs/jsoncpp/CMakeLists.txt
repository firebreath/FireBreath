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

project(jsoncpp)

if (TARGET jsoncpp)
    set(TARGET_ALREADY_EXISTS 1)
else()
    set(TARGET_ALREADY_EXISTS 0)
endif()

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${ScriptingCore_SOURCE_DIR}
    ${FB_CONFIG_DIR}
    ${Boost_INCLUDE_DIRS}
)

file (GLOB GENERAL_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/*.cpp
    src/*.inl
    include/json/*.h
)

file (GLOB FB_HELPERS RELTAIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp
    *.h
)

SOURCE_GROUP("FB Helpers" FILES ${FB_HELPERS})

list(APPEND SOURCES
    ${GENERAL_SRC}
    ${FB_HELPERS}
)

if( CMAKE_COMPILER_IS_GNUCXX )
  #Get compiler version.
  execute_process( COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
                   OUTPUT_VARIABLE GNUCXX_VERSION )
 
  #-Werror=* was introduced -after- GCC 4.1.2
  if( GNUCXX_VERSION VERSION_GREATER 4.1.2 )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=strict-aliasing")
  endif()
endif( CMAKE_COMPILER_IS_GNUCXX )

if (NOT TARGET_ALREADY_EXISTS)
    add_library (jsoncpp STATIC ${SOURCES})
    set_target_properties(jsoncpp PROPERTIES FOLDER "Libraries")
endif()

if (FIREBREATH)
    append_firebreath_link_library(jsoncpp)

    append_firebreath_include_dir(${CMAKE_CURRENT_SOURCE_DIR}/include)

    export_project_dependencies()
endif()
