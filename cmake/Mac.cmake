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

set(SUFFIX ".dylib")

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

    if (VERBOSE)
        message(STATUS "Creating Mac Plugin...")
        message(STATUS "Using Info.plist template ${PLIST_TEMPLATE}")
        message(STATUS "Using InfoPlist.strings template ${STRINGS_TEMPLATE}")
        message(STATUS "Using Localized.r template ${LOCALIZED_TEMPLATE}")
    endif()

    configure_template(${PLIST_TEMPLATE} ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist)
    configure_template(${STRINGS_TEMPLATE} ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/InfoPlist.strings)
    configure_template(${LOCALIZED_TEMPLATE} ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.r)

    #set(MAC_RESOURCE_FILES ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.r)

    if(${ARGC} EQUAL 5)
        set(SOURCES
            ${${INSOURCES}}
            ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist
            ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/InfoPlist.strings
            ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.r
            ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.rsrc
        )
    ELSE()
        set(SOURCES
            ${${INSOURCES}}
            ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist
            ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/InfoPlist.strings
            ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.r
            ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.rsrc
            ${ARGV5}
        )
    ENDIF()

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
        ${RC_COMPILER} ${RCFILES} -useDF ${ARCHS} -arch x86_64 -o ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.rsrc -is ${CMAKE_OSX_SYSROOT}
        )

    set_source_files_properties(
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.rsrc
        PROPERTIES GENERATED 1
        )
    message("Generating ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.rsrc")

    add_library( ${PROJECT_NAME} MODULE
        ${SOURCES} 
        )

    if(${ARGC} EQUAL 5)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            OUTPUT_NAME ${FBSTRING_PluginFileName}
            BUNDLE 1
            BUNDLE_EXTENSION plugin
            XCODE_ATTRIBUTE_WRAPPER_EXTENSION plugin  #sets the extension to .plugin
            XCODE_ATTRIBUTE_MACH_O_TYPE mh_bundle
            XCODE_ATTRIBUTE_INFOPLIST_FILE ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist
            LINK_FLAGS "-Wl,-exported_symbols_list,${FB_ESC_ROOT_DIR}/gen_templates/ExportList_plugin.txt")
    ELSE()
        set_target_properties(${PROJECT_NAME} PROPERTIES
            OUTPUT_NAME ${FBSTRING_PluginFileName}
            BUNDLE 1
            BUNDLE_EXTENSION plugin
            XCODE_ATTRIBUTE_WRAPPER_EXTENSION plugin  #sets the extension to .plugin
            XCODE_ATTRIBUTE_MACH_O_TYPE mh_bundle
            XCODE_ATTRIBUTE_INFOPLIST_FILE ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist
            RESOURCE "${ARGV5}"
            LINK_FLAGS "-Wl,-exported_symbols_list,${FB_ESC_ROOT_DIR}/gen_templates/ExportList_plugin.txt")
    ENDIF()

    set_source_files_properties(
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/InfoPlist.strings
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.rsrc
        PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/English.lproj")

    set(CFGFILE_SCRIPT "${FB_ROOT}/cmake/GenericConfigureFileScript.cmake")
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -D INFILE="${CMAKE_CURRENT_BINARY_DIR}/gen/fwh-chrome-manifest.json"
                                 -D OUTFILE="$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources/fwh-chrome-manifest.template"
                                 -D NAMESHOST="_FILEPATH_"
                                 -P ${CFGFILE_SCRIPT}
        COMMAND ${CMAKE_COMMAND} -D INFILE="${CMAKE_CURRENT_BINARY_DIR}/gen/fwh-mozilla-manifest.json"
                                 -D OUTFILE="$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources/fwh-mozilla-manifest.template"
                                 -D NAMESHOST="_FILEPATH_"
                                 -P ${CFGFILE_SCRIPT}
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/gen/crx_request.json" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources/"
        )

ENDMACRO(add_mac_plugin)

MACRO(firebreath_find_commands)

    set(XCODE_TOOLS_PATHS /Developer/Tools /Applications/Xcode.app/Contents/Developer/Tools /Applications/Xcode.app/Contents/Developer/usr/bin /usr/bin)

    find_program(CMD_CP cp)
    set(CMD_CP ${CMD_CP} CACHE INTERNAL "location of command")
    find_program(CMD_RM rm)
    set(CMD_RM ${CMD_RM} CACHE INTERNAL "location of command")
    find_program(CMD_LN ln)
    set(CMD_LN ${CMD_LN} CACHE INTERNAL "location of command")
    find_program(CMD_MV mv)
    set(CMD_MV ${CMD_MV} CACHE INTERNAL "location of command")
    find_program(CMD_HDIUTIL hdiutil)
    set(CMD_HDIUTIL ${CMD_HDIUTIL} CACHE INTERNAL "location of command")
    find_program(CMD_SIPS sips)
    set(CMD_SIPS ${CMD_SIPS} CACHE INTERNAL "location of command")
    find_program(CMD_SLEEP sleep)
    set(CMD_SLEEP ${CMD_SLEEP} CACHE INTERNAL "location of command")
    find_program(CMD_OSASCRIPT osascript)
    set(CMD_OSASCRIPT ${CMD_OSASCRIPT} CACHE INTERNAL "location of command")
    find_program(CMD_SETFILE SetFile ${XCODE_TOOLS_PATHS} )
    set(CMD_SETFILE ${CMD_SETFILE} CACHE INTERNAL "location of command")
    find_program(CMD_DEREZ DeRez ${XCODE_TOOLS_PATHS} )
    set(CMD_DEREZ ${CMD_DEREZ} CACHE INTERNAL "location of command")
    find_program(CMD_REZ Rez ${XCODE_TOOLS_PATHS} )
    set(CMD_REZ ${CMD_REZ} CACHE INTERNAL "location of command")
    find_program(CMD_PKGBUILD pkgbuild)
    set(CMD_PKGBUILD ${CMD_PKGBUILD} CACHE INTERNAL "location of command")
    find_program(CMD_PRODUCTBUILD productbuild)
    set(CMD_PRODUCTBUILD ${CMD_PRODUCTBUILD} CACHE INTERNAL "location of command")
    
ENDMACRO(firebreath_find_commands)

macro(firebreath_find_plugin_path OUTPATH PROJNAME)
    cmake_policy(SET CMP0026 OLD)
    get_property(NEXTPATH TARGET ${PROJNAME} PROPERTY LOCATION)
    
    while (${NEXTPATH} MATCHES "\\.plugin")
        set(PLUGINPATH ${NEXTPATH})
        get_filename_component(NEXTPATH "${PLUGINPATH}/.." ABSOLUTE)
    endwhile()
    set(${OUTPATH} ${PLUGINPATH})
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
