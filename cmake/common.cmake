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

if (WIN32)
    set (PLATFORM_NAME "Win")
elseif(APPLE)
    set (PLATFORM_NAME "Mac")
elseif(UNIX)
    set (PLATFORM_NAME "Linux")
endif()

if (WIN32)
    include(${CMAKE_DIR}/wix.cmake)
endif(WIN32)

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

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_NAME}/projectDef.cmake)
        include(${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_NAME}/projectDef.cmake)
    else()
        message ("Could not find a ${PLATFORM_NAME} directory for the current project")
    endif()

endfunction(include_platform)

function (add_wix_installer PROJNAME WIX_SOURCEFILES WIX_COMPGROUP WIX_OUTDIR WIX_DLLFILES WIX_PROJDEP)
    if (WIN32 AND WIX_FOUND)
        set(SOURCELIST )
        FOREACH(_curFile ${WIX_SOURCEFILES})
            GET_FILENAME_COMPONENT(_tmp_File ${_curFile} NAME)
            configure_file(${_curFile} ${CMAKE_CURRENT_BINARY_DIR}/${_tmp_File})
            message("Configuring ${_curFile} -> ${CMAKE_CURRENT_BINARY_DIR}/${_tmp_File}")
            set(SOURCELIST ${SOURCELIST} ${CMAKE_CURRENT_BINARY_DIR}/${_tmp_File})
        ENDFOREACH()
        
        set (WIX_HEAT_FLAGS ${WIX_HEAT_FLAGS} -var var.BINSRC "-t:${CMAKE_DIR}\\FixFragment.xslt")
        set (WIX_CANDLE_FLAGS ${WIX_LINK_FLAGS} -dBINSRC=${WIX_OUTDIR})
        set (WIX_LINK_FLAGS ${WIX_LINK_FLAGS} -sw1076)
        WIX_HEAT(WIX_DLLFILES WIXDLLWXS_LIST NONE)
        set (SOURCELIST ${SOURCELIST} ${WIXDLLWXS_LIST})
        WIX_COMPILE(SOURCELIST WIXOBJ_LIST WIXDLLWXS_LIST)
        SET (WIX_FULLOBJLIST ${WIXOBJ_LIST} )

        SET (WIX_DEST ${WIX_OUTDIR}/${PROJNAME}.msi)

        set_source_files_properties(${SOURCELIST} PROPERTIES HEADER_FILE_ONLY 1)
        SOURCE_GROUP(Sources FILES ${WIX_SOURCEFILES})
        SOURCE_GROUP(Generated FILES ${SOURCELIST})
        set_source_files_properties(${WIXOBJ_LIST} ${WIX_DEST} PROPERTIES GENERATED 1)
        SOURCE_GROUP(Binary FILES ${WIXOBJ_LIST})
        set (WIX_SOURCES
                ${CMAKE_DIR}/dummy.cpp
                ${WIX_SOURCEFILES}
                ${SOURCELIST}
                ${WIXOBJ_LIST}
            )
        ADD_LIBRARY(${PROJNAME}_WiXInstall STATIC ${WIX_SOURCES})

        WIX_LINK(${PROJNAME}_WiXInstall ${WIX_DEST} WIX_FULLOBJLIST NONE)

        ADD_DEPENDENCIES(${PROJNAME}_WiXInstall ${WIX_PROJDEP})

    endif()
endfunction(add_wix_installer)
