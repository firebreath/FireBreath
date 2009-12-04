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

# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

get_filename_component (CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

include (${CMAKE_DIR}/common.cmake)

if (GEN)
    set(GEN_ARG -G "${GEN}")
endif()

execute_process(COMMAND "${CMAKE_COMMAND}"
    ${GEN_ARG}
    -D "CMAKE_DIR:PATH=${CMAKE_DIR}"

    ${CMAKE_CURRENT_SOURCE_DIR}
    WORKING_DIRECTORY ${BUILD_DIR}
    )
