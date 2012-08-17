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

if (NOT VC_DIR)
    if (MSVC10)
        GET_FILENAME_COMPONENT(VS_DIR "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\10.0\\Setup\\VS;ProductDir]" REALPATH CACHE)
    elseif (MSVC90)
        GET_FILENAME_COMPONENT(VS_DIR "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\9.0\\Setup\\VS;ProductDir]" REALPATH CACHE)
    elseif (MSVC80)
        GET_FILENAME_COMPONENT(VS_DIR "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\8.0\\Setup\\VS;ProductDir]" REALPATH CACHE)
    else()
        message("!! Could not find VS DIR!")
    endif()
    set(VC_DIR ${VS_DIR}/VC CACHE STRING "Visual Studio VC directory path")
    message("-- Visual Studio dir: ${VS_DIR}")
endif()
if (NOT ATL_INCLUDE_DIR)
    set (DDK_SEARCH_PATHS 
        "$ENV{DDK_PATH}"
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
    NO_DEFAULT_PATH
    )

    if (CMAKE_SIZEOF_VOID_P EQUAL 8)        
        set(ATLLIB_GUESSES "${VC_DIR}/atlmfc/lib/amd64" ${ATLLIB_GUESSES})
    else()
        set(ATLLIB_GUESSES "${VC_DIR}/atlmfc/lib" ${ATLLIB_GUESSES})
    endif()
    find_file(ATLLIB
        atls.lib
    PATHS
        ${ATLLIB_GUESSES}
        NO_DEFAULT_PATH
    )

    find_file(MFCWIN
        winres.h
    PATHS
        ${VC_DIR}/atlmfc/include
        ${MFC_GUESSES}
    NO_DEFAULT_PATH
    )

    if (ATLWIN AND ATLLIB)
        GET_FILENAME_COMPONENT(ATL_INCLUDE_DIR ${ATLWIN} PATH CACHE)
        GET_FILENAME_COMPONENT(ATL_LIBRARY_DIR ${ATLLIB} PATH CACHE)
        find_file(ATL_LIBRARY
            atlthunk.lib
        PATHS
            ${ATL_LIBRARY_DIR}
        NO_DEFAULT_PATH
            )
        if (ATL_LIBRARY)
            set (ATL_LIBRARY "atlthunk.lib" CACHE INTERNAL "Used to link extra ATL libraries on systems that need it")
        else()
            set(ATL_LIBRARY "" CACHE INTERNAL "Used to link extra ATL libraries on systems that need it")
        endif()
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
        OUTPUT_NAME ${FBSTRING_PluginFileName}
        PROJECT_LABEL ${PROJNAME}
        RUNTIME_OUTPUT_DIRECTORY "${FB_BIN_DIR}/${PLUGIN_NAME}"
        LIBRARY_OUTPUT_DIRECTORY "${FB_BIN_DIR}/${PLUGIN_NAME}"
        LINK_FLAGS "${LINK_FLAGS}"
        )

    target_link_libraries(${PROJNAME} psapi)

ENDMACRO(add_windows_plugin)

macro(firebreath_sign_file PROJNAME _FILENAME PFXFILE PASSFILE TIMESTAMP_URL)
    if (WIN32)
        if (EXISTS ${PFXFILE})
            message("-- ${_FILENAME} will be signed with ${PFXFILE}")
            GET_FILENAME_COMPONENT(WINSDK_DIR "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows;CurrentInstallFolder]" REALPATH CACHE)
            find_program(SIGNTOOL signtool
                PATHS
                ${WINSDK_DIR}/bin
                )
            if (SIGNTOOL)
                set(_STCMD signtool sign /f "${PFXFILE}")
                if (NOT "${PASSFILE}" STREQUAL "")
                    file(STRINGS "${PASSFILE}" PASSPHRASE LIMIT_COUNT 1)
                    set(_STCMD ${_STCMD} /p ${PASSPHRASE})
                endif()
                if (NOT "${TIMESTAMP_URL}" STREQUAL "")
                    set(_STCMD ${_STCMD} /t "${TIMESTAMP_URL}")
                endif()
                set(_STCMD ${_STCMD} "${_FILENAME}")
                ADD_CUSTOM_COMMAND(
                    TARGET ${PROJNAME}
                    POST_BUILD
                    COMMAND ${_STCMD}
                    )
                message("-- Successfully added signtool step to sign ${_FILENAME}")
            else()
                message("Could not find signtool! Code signing disabled ${SIGNTOOL}")
            endif()
            set(PASSPHRASE "")
        else()
            message("-- No signtool certificate found; assuming development machine (${PFXFILE})")
        endif()

    endif()
endmacro(firebreath_sign_file)

macro(firebreath_sign_plugin PROJNAME PFXFILE PASSFILE TIMESTAMP_URL)
    if (WIN32)
        get_target_property(ONAME ${PROJNAME} OUTPUT_NAME)
        get_target_property(LIBDIR ${PROJNAME} LIBRARY_OUTPUT_DIRECTORY)

        set(_PLUGFILENAME "${LIBDIR}/${CMAKE_CFG_INTDIR}/${ONAME}.dll")
        firebreath_sign_file("${PROJNAME}" "${_PLUGFILENAME}" "${PFXFILE}" "${PASSFILE}" "${TIMESTAMP_URL}")
    endif()
endmacro(firebreath_sign_plugin)

function (add_wix_installer PROJNAME WIX_SOURCEFILES WIX_COMPGROUP WIX_OUTDIR WIX_DLLFILES WIX_PROJDEP)
    if (WIN32 AND WIX_FOUND)
        set(SOURCELIST )
        FOREACH(_curFile ${WIX_SOURCEFILES})
            GET_FILENAME_COMPONENT(_tmp_File ${_curFile} NAME)
            configure_file(${_curFile} ${CMAKE_CURRENT_BINARY_DIR}/${_tmp_File})
            message("Configuring ${_curFile} -> ${CMAKE_CURRENT_BINARY_DIR}/${_tmp_File}")
            set(SOURCELIST ${SOURCELIST} ${CMAKE_CURRENT_BINARY_DIR}/${_tmp_File})
        ENDFOREACH()

        if (WIX_FORCE_PER STREQUAL "machine")
            set(_WIX_HEAT_TRANSFORM "FixFragment_HKLM.xslt")
        else()
            set(_WIX_HEAT_TRANSFORM "FixFragment_HKCU.xslt")
        endif()

        set (WIX_HEAT_FLAGS ${WIX_HEAT_FLAGS} -var var.BINSRC "-t:${FB_ROOT}\\cmake\\${_WIX_HEAT_TRANSFORM}")
        set (WIX_CANDLE_FLAGS ${WIX_LINK_FLAGS} -dBINSRC=${WIX_OUTDIR} -dPLUGINSRC=${FB_CURRENT_PLUGIN_DIR})
        set (WIX_LINK_FLAGS ${WIX_LINK_FLAGS} -sw1076)
        WIX_HEAT(WIX_DLLFILES WIXDLLWXS_LIST NONE)
        set (SOURCELIST ${SOURCELIST} ${WIXDLLWXS_LIST})
        WIX_COMPILE(SOURCELIST WIXOBJ_LIST WIXDLLWXS_LIST)
        SET (WIX_FULLOBJLIST ${WIXOBJ_LIST} )

        if (FB_WIX_DEST)
            SET (WIX_DEST ${FB_WIX_DEST})
        else()
            SET (WIX_DEST ${WIX_OUTDIR}/${PROJNAME}.msi)
        endif()

        set_source_files_properties(${SOURCELIST} PROPERTIES HEADER_FILE_ONLY 1)
        SOURCE_GROUP(Sources FILES ${WIX_SOURCEFILES})
        SOURCE_GROUP(Generated FILES ${SOURCELIST})
        set_source_files_properties(${WIXOBJ_LIST} ${WIX_DEST} PROPERTIES GENERATED 1)
        SOURCE_GROUP(Binary FILES ${WIXOBJ_LIST})
        set (WIX_SOURCES
                ${FB_ROOT}/cmake/dummy.cpp
                ${WIX_SOURCEFILES}
                ${SOURCELIST}
                ${WIXOBJ_LIST}
            )
        if (NOT FB_WIX_SUFFIX)
            set (FB_WIX_SUFFIX _WiXInstall)
        endif()
        ADD_LIBRARY(${PROJNAME}${FB_WIX_SUFFIX} STATIC ${WIX_SOURCES})

        WIX_LINK(${PROJNAME}${FB_WIX_SUFFIX} ${WIX_DEST} WIX_FULLOBJLIST NONE)

        ADD_DEPENDENCIES(${PROJNAME}${FB_WIX_SUFFIX} ${WIX_PROJDEP})

    endif()
endfunction(add_wix_installer)
