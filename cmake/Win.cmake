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

