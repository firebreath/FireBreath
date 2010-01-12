#/**********************************************************\ 
#Original Author: Richard Bateman (taxilian)
#
#Created:    Jan 11, 2010
#License:    Dual license model; choose one of two:
#            Eclipse Public License - Version 1.0
#            http://www.eclipse.org/legal/epl-v10.html
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#            
#Copyright 2009 PacketPass, Inc and the Firebreath development team
#\**********************************************************/

if ("${CMAKE_GENERATOR}" STREQUAL "Xcode" AND NOT XCODE_DIR)
    execute_process (COMMAND
         xcode-select -print-path
         RESULT_VARIABLE XCODE_RES
         OUTPUT_VARIABLE XCODE_DIR
         ERROR_QUIET
         OUTPUT_STRIP_TRAILING_WHITESPACE)

    if (NOT XCODE_RES EQUAL 0)
        set (XCODE_DIR "/Developer")
        message ("Could not find Xcode.  Defaulting to ${XCODE_DIR}")
    endif()

    set (XCODE_DIR "${XCODE_DIR}" CACHE PATH "Path to Xcode")
endif()

MACRO(add_mac_plugin PROJECT_NAME PLIST_TEMPLATE STRINGS_TEMPLATE LOCALIZED_TEMPLATE SOURCES)

    message ("Creating Mac Browser Plugin project ${PROJECT_NAME}")
    if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/bundle)
        file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bundle)
        file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj)
    endif()

    configure_file(${PLIST_TEMPLATE} ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist)
    configure_file(${STRINGS_TEMPLATE} ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/InfoPlist.strings)
    configure_file(${LOCALIZED_TEMPLATE} ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.r)

    #set(MAC_RESOURCE_FILES ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.r)

    message("${SOURCES} = ${${SOURCES}}")
    set(SOURCES
        ${${SOURCES}}
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/InfoPlist.strings
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.r
    )

    add_definitions(
        -DXP_MACOSX=1
        )

    add_executable( ${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES} )

    set_target_properties(${PROJECT_NAME} PROPERTIES
        XCODE_ATTRIBUTE_WRAPPER_EXTENSION plugin  #sets the extension to .plugin
        XCODE_ATTRIBUTE_MACH_O_TYPE mh_bundle
        XCODE_ATTRIBUTE_INFOPLIST_FILE ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist
        LINK_FLAGS "-Wl,-exported_symbols_list,${FB_ROOT_DIR}/gen_templates/ExportList_plugin.txt ${XCODE_LINK_FLAGS}")

    set (RCFILES ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.r)
    # Compile the resource file
    find_program(RC_COMPILER Rez NO_DEFAULT_PATHS)
    message("Found ${RC_COMPILER}")
    if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/rcgen.sh)
        configure_file( ${FB_ROOT_DIR}/gen_templates/rcgen.sh.in ${CMAKE_CURRENT_BINARY_DIR}/rcgen.sh )
    endif()

    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND /bin/bash ${CMAKE_CURRENT_BINARY_DIR}/rcgen.sh
        )

    set_source_Files_properties(
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/InfoPlist.strings
        PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/English.lproj")

ENDMACRO(add_mac_plugin)

