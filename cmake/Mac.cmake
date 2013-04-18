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

# The reason 2.8.5 is required as a minimum version on Mac is that 2.8.5 is where the patch to allow
# direct compilation of cfbundle projects in Xcode and gcc was introduced into cmake.
cmake_minimum_required(VERSION 2.8.5)

set (PATCH_DESC_FILENAME ${CMAKE_BINARY_DIR}/xcode_patch_desc.txt)

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

function(patch_xcode_plugin PRJNAME TARGETNAME)

    file(APPEND ${PATCH_DESC_FILENAME} "${TARGETNAME} ${PRJNAME}\n")

endfunction(patch_xcode_plugin)

function(clear_xcode_patches)

    if (APPLE)
        if (EXISTS ${PATCH_DESC_FILENAME})
            file(REMOVE ${PATCH_DESC_FILENAME})
        endif()
    endif(APPLE)

endfunction(clear_xcode_patches)

MACRO(add_mac_plugin PROJECT_NAME PLIST_TEMPLATE STRINGS_TEMPLATE LOCALIZED_TEMPLATE INSOURCES)

    message ("Creating Mac Browser Plugin project ${PROJECT_NAME}")
    if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/bundle)
        file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bundle)
        file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj)
    endif()

    configure_template(${PLIST_TEMPLATE} ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist)
    configure_template(${STRINGS_TEMPLATE} ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/InfoPlist.strings)
    configure_template(${LOCALIZED_TEMPLATE} ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.r)

    #set(MAC_RESOURCE_FILES ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.r)

    set(SOURCES
        ${${INSOURCES}}
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/InfoPlist.strings
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.r
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.rsrc
    )

    add_definitions(
        -DXP_MACOSX=1
        )

    set (RCFILES ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.r)

    foreach(ARCH ${CMAKE_OSX_ARCHITECTURES})
        list(APPEND ARCHS -arch ${ARCH})
    endforeach()

    # Compile the resource file
    firebreath_find_commands()
    set(RC_COMPILER ${CMD_REZ})
    execute_process(COMMAND
        ${RC_COMPILER} ${RCFILES} -useDF ${ARCHS} -arch x86_64 -o ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.rsrc
        )

    set_source_files_properties(
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.rsrc
        PROPERTIES GENERATED 1
        )
    message("Generating ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.rsrc")

    add_library( ${PROJECT_NAME} MODULE
        ${SOURCES} 
        )

    string(REPLACE " " "\ " FB_ESC_ROOT_DIR ${FB_ROOT_DIR})
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME ${FBSTRING_PluginFileName}
        BUNDLE 1
        BUNDLE_EXTENSION plugin
        XCODE_ATTRIBUTE_WRAPPER_EXTENSION plugin  #sets the extension to .plugin
        XCODE_ATTRIBUTE_MACH_O_TYPE mh_bundle
        XCODE_ATTRIBUTE_INFOPLIST_FILE ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist
        LINK_FLAGS "-Wl,-exported_symbols_list,${FB_ESC_ROOT_DIR}/gen_templates/ExportList_plugin.txt")

    set_source_files_properties(
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/InfoPlist.strings
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.rsrc
        PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/English.lproj")

ENDMACRO(add_mac_plugin)

MACRO(firebreath_find_commands)

    set(XCODE_TOOLS_PATHS /Developer/Tools /Applications/Xcode.app/Contents/Developer/Tools /Applications/Xcode.app/Contents/Developer/usr/bin /usr/bin)

    find_program(CMD_CP cp)
    find_program(CMD_RM rm)
    find_program(CMD_LN ln)
    find_program(CMD_MV mv)
    find_program(CMD_HDIUTIL hdiutil)
    find_program(CMD_SIPS sips)
    find_program(CMD_SLEEP sleep)
    find_program(CMD_OSASCRIPT osascript)
    find_program(CMD_SETFILE SetFile ${XCODE_TOOLS_PATHS} )
    find_program(CMD_DEREZ DeRez ${XCODE_TOOLS_PATHS} )
    find_program(CMD_REZ Rez ${XCODE_TOOLS_PATHS} )

ENDMACRO(firebreath_find_commands)

macro(firebreath_find_plugin_path OUTPATH PROJNAME)
    get_property(${OUTPATH} TARGET ${PROJNAME} PROPERTY LOCATION)
    while (${OUTPATH} MATCHES "\\.plugin")
        get_filename_component(${OUTPATH} "${${OUTPATH}}/.." ABSOLUTE)
    endwhile()
    get_target_property(ONAME ${PROJNAME} OUTPUT_NAME)
    set(${OUTPATH} "${${OUTPATH}}/${ONAME}.plugin")
endmacro(firebreath_find_plugin_path)

macro(firebreath_sign_file PROJNAME FILETOSIGN IDENTITY)

    message("Project: ${PROJNAME}")
    find_program(MAC_CODESIGN codesign)
    set(_STCMD "${MAC_CODESIGN}" -s "${IDENTITY}" -v "${FILETOSIGN}")
    ADD_CUSTOM_COMMAND(
        TARGET ${PROJNAME}
        POST_BUILD
        COMMAND ${_STCMD}
        )
    message(STATUS "Enabled codesign step to sign ${FILETOSIGN}")

endmacro(firebreath_sign_file)

macro(firebreath_sign_plugin PROJNAME IDENTITY)

    message("Project: ${PROJNAME}")
    get_target_property(ONAME ${PROJNAME} OUTPUT_NAME)
    firebreath_find_plugin_path(_PLUGFILENAME ${PROJNAME})

    firebreath_sign_file(${PROJNAME} "${_PLUGFILENAME}" "${IDENTITY}")

endmacro(firebreath_sign_plugin)
