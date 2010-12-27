#/**********************************************************\ 
#Original Author: Richard Bateman (taxilian)
#
#Created:    Jan 11, 2010
#License:    Dual license model; choose one of two:
#            New BSD License
#            http://www.opensource.org/licenses/bsd-license.php
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#            
#Copyright 2009 PacketPass, Inc and the Firebreath development team
#\**********************************************************/

# Find ATL stuff

if (NOT $ENV{VS100COMNTOOLS} STREQUAL "")
    GET_FILENAME_COMPONENT(VC_DIR "$ENV{VS100COMNTOOLS}/../../VC" REALPATH CACHE)
elseif(NOT $ENV{VS90COMNTOOLS} STREQUAL "")
    GET_FILENAME_COMPONENT(VC_DIR "$ENV{VS90COMNTOOLS}/../../VC" REALPATH CACHE)
elseif(NOT $ENV{VS80COMNTOOLS} STREQUAL "")
    GET_FILENAME_COMPONENT(VC_DIR "$ENV{VS80COMNTOOLS}/../../VC" REALPATH CACHE)
endif()

if (NOT ATL_INCLUDE_DIR)
    set (DDK_SEARCH_PATHS 
        "$ENV{SystemDrive}/WinDDK"
        "$ENV{ProgramFiles}/WinDDK"
        "$ENV{CommonProgramFiles}/WinDDK"
        CACHE INTERNAL "DDK Search Paths")

    set(ATL_GUESSES ".")
    foreach (SEARCH_DIR ${DDK_SEARCH_PATHS})
        if (EXISTS ${SEARCH_DIR})
            file (GLOB DDK_DIRS
                ${SEARCH_DIR}/*)
            foreach(DDKDIR ${DDK_DIRS})
                file(GLOB ATL_CUR
                    ${DDKDIR}/inc/atl*)
                SET(ATL_GUESSES ${ATL_GUESSES} ${ATL_CUR})
            endforeach()
        endif()
    endforeach()

    set(ATLLIB_GUESSES ".")
    foreach (SEARCH_DIR ${DDK_SEARCH_PATHS})
        if (EXISTS ${SEARCH_DIR})
            file (GLOB DDK_DIRS
                ${SEARCH_DIR}/*)
            foreach(DDKDIR ${DDK_DIRS})
                file(GLOB ATL_CUR
                    ${DDKDIR}/lib/atl/i386)
                SET(ATLLIB_GUESSES ${ATLLIB_GUESSES} ${ATL_CUR})
            endforeach()
        endif()
    endforeach()

    set(MFC_GUESSES ".")
    foreach (SEARCH_DIR ${DDK_SEARCH_PATHS})
        if (EXISTS ${SEARCH_DIR})
            file (GLOB DDK_DIRS
                ${SEARCH_DIR}/*)
            foreach(DDKDIR ${DDK_DIRS})
                file(GLOB MFC_CUR
                    ${DDKDIR}/inc/mfc*)
                SET(MFC_GUESSES ${MFC_GUESSES} ${MFC_CUR})
            endforeach()
        endif()
    endforeach()

    find_file(ATLWIN
        atlwin.h
    PATHS
        ${VC_DIR}/atlmfc/include
        ${ATL_GUESSES}
    )

    find_file(ATLLIB
        atls.lib
    PATHS
        ${VC_DIR}/atlmfc/lib
        ${ATLLIB_GUESSES}
    )

    find_file(MFCWIN
        winres.h
    PATHS
        ${VC_DIR}/atlmfc/include
        ${MFC_GUESSES}
    )

    if (ATLWIN AND ATLLIB)
        GET_FILENAME_COMPONENT(ATL_INCLUDE_DIR ${ATLWIN} PATH CACHE)
        GET_FILENAME_COMPONENT(ATL_LIBRARY_DIR ${ATLLIB} PATH CACHE)
        message("-- Found ATL include dir: ${ATL_INCLUDE_DIR}")
        message("-- Found ATL lib dir: ${ATL_LIBRARY_DIR}")
    else()
        message(FATAL_ERROR "FireBreath on windows requires ATL/MFC libs to be installed.  Please download the Microsoft DDK and install the build environments in $ENV{SystemDrive}\\WinDDK")
    endif()
    if (MFCWIN)
        GET_FILENAME_COMPONENT(MFC_INCLUDE_DIR ${MFCWIN} PATH CACHE)
        message("-- Found MFC include dir: ${MFC_INCLUDE_DIR}")
    else()
        message(FATAL_ERROR "FireBreath on windows requires ATL/MFC libs to be installed.  Please download the Microsoft DDK and install the build environments in $ENV{SystemDrive}\\WinDDK")
    endif()

endif()
set(ATL_INCLUDE_DIRS
    ${ATL_INCLUDE_DIR}
    ${MFC_INCLUDE_DIR}
    CACHE INTERNAL "ATL and MFC include dirs")


MACRO(add_windows_plugin PROJNAME INSOURCES)
    set(SOURCES
        ${${INSOURCES}}
    )

    add_library(${PROJNAME} SHARED ${SOURCES})

    set_target_properties (${PROJNAME} PROPERTIES
        OUTPUT_NAME np${PLUGIN_NAME}
        PROJECT_LABEL ${PROJNAME}
        RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}/${PLUGIN_NAME}"
        LIBRARY_OUTPUT_DIRECTORY "${BIN_DIR}/${PLUGIN_NAME}"
        LINK_FLAGS "${LINK_FLAGS}"
        )

ENDMACRO(add_windows_plugin)


function (add_wix_installer PROJNAME WIX_SOURCEFILES WIX_COMPGROUP WIX_OUTDIR WIX_DLLFILES WIX_PROJDEP)
	# WiX doesn't work with VC8 generated DLLs
    if (WIN32 AND WIX_FOUND AND NOT CMAKE_GENERATOR STREQUAL "Visual Studio 8 2005")
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

