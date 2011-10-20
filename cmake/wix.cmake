# - Try to find Windows Installer XML
# See http://wix.sourceforge.net
#
# The follwoing variables are optionally searched for defaults
#  WIX_ROOT_DIR:            Base directory of WIX2 tree to use.
#
# The following are set after configuration is done: 
#  WIX_FOUND
#  WIX_ROOT_DIR
#  WIX_CANDLE
#  WIX_LIGHT
# 
# 2009/02 Petr Pytelka (pyta at lightcomp.cz)
#

if (WIN32)
    MACRO(DBG_MSG _MSG)
        if (VERBOSE)
            MESSAGE(STATUS "${CMAKE_CURRENT_LIST_FILE}(${CMAKE_CURRENT_LIST_LINE}):\r\n ${_MSG}")
        endif()
    ENDMACRO(DBG_MSG)


    if (NOT WIX_ROOT_DIR)
        # typical root dirs of installations, exactly one of them is used
        SET (WIX_POSSIBLE_ROOT_DIRS
            "${WIX_ROOT_DIR}"
            "$ENV{WIX}"
            "$ENV{WIX_ROOT_DIR}"
            "$ENV{ProgramFiles}/Windows Installer XML"
            "$ENV{ProgramFiles}/Windows Installer XML v3"
            "$ENV{ProgramFiles}/Windows Installer XML v3.5"
            "$ENV{ProgramFiles}/Windows Installer XML v3.6"
            )


        #DBG_MSG("DBG (WIX_POSSIBLE_ROOT_DIRS=${WIX_POSSIBLE_ROOT_DIRS}")

        #
        # select exactly ONE WIX base directory/tree 
        # to avoid mixing different version headers and libs
        #
        FIND_PATH(WIX_ROOT_DIR 
            NAMES 
            bin/candle.exe
            bin/light.exe
            bin/heat.exe
            PATHS ${WIX_POSSIBLE_ROOT_DIRS})
        DBG_MSG("WIX_ROOT_DIR=${WIX_ROOT_DIR}")
    endif()


    #
    # Logic selecting required libs and headers
    #
    SET(WIX_FOUND OFF)
    IF(WIX_ROOT_DIR)
        SET(WIX_FOUND ON)
    ENDIF(WIX_ROOT_DIR)


    # display help message
    IF(NOT WIX_FOUND)
        # make FIND_PACKAGE friendly
        IF(NOT WIX_FIND_QUIETLY)
            IF(WIX_FIND_REQUIRED)
                MESSAGE(FATAL_ERROR
                    "Windows Installer XML required but some files not found. Please specify it's location with WIX_ROOT_DIR env. variable.")
            ENDIF(WIX_FIND_REQUIRED)
        ENDIF(NOT WIX_FIND_QUIETLY)
    ELSE(NOT WIX_FOUND)
        SET(WIX_CANDLE ${WIX_ROOT_DIR}/bin/candle.exe)
        SET(WIX_LIGHT ${WIX_ROOT_DIR}/bin/light.exe)
        SET(WIX_HEAT ${WIX_ROOT_DIR}/bin/heat.exe)
        #  MESSAGE(STATUS "Windows Installer XML found.")
    ENDIF(NOT WIX_FOUND)


    MARK_AS_ADVANCED(
        WIX_ROOT_DIR
        WIX_CANDLE
        WIX_LIGHT
        WIX_HEAT
        )

    #
    # Call wix compiler
    #
    # Parameters:
    #  _sources - name of list with sources
    #  _obj - name of list for target objects
    #
    MACRO(WIX_COMPILE _sources _objs _extra_dep)
        DBG_MSG("WIX compile: ${${_sources}}")
        FOREACH (_current_FILE ${${_sources}})
            GET_FILENAME_COMPONENT(_tmp_FILE ${_current_FILE} ABSOLUTE)
            GET_FILENAME_COMPONENT(_basename ${_tmp_FILE} NAME_WE)

            SET (SOURCE_WIX_FILE ${_tmp_FILE} )
            DBG_MSG("WIX source file: ${SOURCE_WIX_FILE}")

            SET (OUTPUT_WIXOBJ ${_basename}.wixobj )

            DBG_MSG("WIX output: ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_WIXOBJ}")
            DBG_MSG("WIX command: ${WIX_CANDLE}")
            SET(EXT_FLAGS -ext WixUtilExtension)
            SET(EXT_FLAGS ${EXT_FLAGS} -ext WixUIExtension)

            ADD_CUSTOM_COMMAND( 
                OUTPUT    ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_WIXOBJ}
                COMMAND   ${WIX_CANDLE}
                ARGS      ${WIX_CANDLE_FLAGS} ${EXT_FLAGS} ${SOURCE_WIX_FILE}
                DEPENDS   ${SOURCE_WIX_FILE} ${${_extra_dep}}
                COMMENT   "Compiling ${SOURCE_WIX_FILE} -> ${OUTPUT_WIXOBJ}"
                )
            SET(${_objs} ${${_objs}} ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_WIXOBJ} )
            DBG_MSG("WIX compile output: ${${_objs}}")

        ENDFOREACH (_current_FILE)
    ENDMACRO(WIX_COMPILE)

    #
    # Call wix heat command for the specified DLLs
    #
    # Parameters:
    #  _sources - name of list with DLLs
    #  _obj - name of list for target objects
    #
    MACRO(WIX_HEAT _sources _objs)
        DBG_MSG("WiX heat: ${${_sources}}")
        FOREACH (_current_DLL ${${_sources}})
            GET_FILENAME_COMPONENT(_tmp_FILE ${_current_DLL} ABSOLUTE)
            GET_FILENAME_COMPONENT(_basename ${_tmp_FILE} NAME_WE)

            SET (SOURCE_WIX_FILE ${_tmp_FILE} )

            if (NOT WIX_HEAT_SUFFIX)
                set(WIX_HEAT_SUFFIX "_auto")
            endif()

            SET (OUTPUT_WIXOBJ ${_basename}${WIX_HEAT_SUFFIX}.wxs )

            DBG_MSG("WIX output: ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_WIXOBJ}")
            DBG_MSG("WIX command: ${WIX_HEAT}")

            ADD_CUSTOM_COMMAND( 
                OUTPUT    ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_WIXOBJ}
                COMMAND   ${WIX_HEAT}
                ARGS      file ${SOURCE_WIX_FILE}
                          -out ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_WIXOBJ}
                          ${WIX_HEAT_FLAGS}
                DEPENDS   ${SOURCE_WIX_FILE}
                COMMENT   "Compiling ${SOURCE_WIX_FILE} -> ${OUTPUT_WIXOBJ}"
                )
            SET(${_objs} ${${_objs}} ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_WIXOBJ} )
        ENDFOREACH(_current_DLL)
        DBG_MSG("WIX compile output: ${${_objs}}")
    ENDMACRO(WIX_HEAT)

    #
    # Call wix compiler
    #
    # Parameters:
    #  _sources - name of list with sources
    #  _obj - name of list for target objects
    #
    MACRO(WIX_COMPILE_ALL _target _sources _extra_dep)
        DBG_MSG("WIX compile all: ${${_sources}}, dependencies: ${${_extra_dep}}")
        SET(EXT_FLAGS -ext WixUtilExtension)
        SET(EXT_FLAGS ${EXT_FLAGS} -ext WixUIExtension)

        ADD_CUSTOM_COMMAND( 
            OUTPUT    ${_target}
            COMMAND   ${WIX_CANDLE}
            ARGS      ${WIX_CANDLE_FLAGS} ${EXT_FLAGS} -out "${_target}" ${${_sources}}
            DEPENDS   ${${_sources}} ${${_extra_dep}}
            COMMENT   "Compiling ${${_sources}} -> ${_target}"
            )

    ENDMACRO(WIX_COMPILE_ALL)


    #
    # Link MSI file as post-build action
    #
    # Parameters
    #  _target - Name of target file
    #  _sources - Name of list with sources
    #
    MACRO(WIX_LINK _project _target _sources _loc_files)
        #DBG_MSG("WIX command: ${WIX_LIGHT}\n WIX target: ${_target} objs: ${${_sources}}")

        SET( WIX_LINK_FLAGS_A ${WIX_LINK_FLAGS} )
        SET(EXT_FLAGS -ext WixUtilExtension)
        SET(EXT_FLAGS ${EXT_FLAGS} -ext WixUIExtension)

        # Add localization
        FOREACH (_current_FILE ${${_loc_files}})
            SET( WIX_LINK_FLAGS_A ${WIX_LINK_FLAGS_A} -loc "${_current_FILE}" )
            DBG_MSG("WIX link localization: ${_current_FILE}")
        ENDFOREACH (_current_FILE)
        DBG_MSG("WIX link flags: ${WIX_LINK_FLAGS_A}")

        ADD_CUSTOM_COMMAND( TARGET    ${_project} POST_BUILD
            COMMAND   ${WIX_LIGHT}
            ARGS      ${WIX_LINK_FLAGS_A} ${EXT_FLAGS} -out "${_target}" ${${_sources}}
            DEPENDS   ${${_sources}}
            COMMENT   "Linking ${${_sources}} -> ${_target}"
            )

    ENDMACRO(WIX_LINK)
endif(WIN32)
