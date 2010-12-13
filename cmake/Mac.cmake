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
    )

    add_definitions(
        -DXP_MACOSX=1
        )

    add_executable( ${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES} )

    string(REPLACE " " "\ " FB_ESC_ROOT_DIR ${FB_ROOT_DIR})
    set_target_properties(${PROJECT_NAME} PROPERTIES
        XCODE_ATTRIBUTE_WRAPPER_EXTENSION plugin  #sets the extension to .plugin
        XCODE_ATTRIBUTE_MACH_O_TYPE mh_bundle
        XCODE_ATTRIBUTE_INFOPLIST_FILE ${CMAKE_CURRENT_BINARY_DIR}/bundle/Info.plist
        LINK_FLAGS "\"-Wl,-exported_symbols_list,${FB_ESC_ROOT_DIR}/gen_templates/ExportList_plugin.txt\"")

    set (RCFILES ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.r)

    foreach(ARCH ${CMAKE_OSX_ARCHITECTURES})
        list(APPEND ARCHS -arch ${ARCH})
    endforeach()

    find_program(RC_COMPILER Rez NO_DEFAULT_PATHS)
    execute_process(COMMAND
        ${RC_COMPILER} ${RCFILES} -useDF ${ARCHS} -arch x86_64 -o ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/Localized.rsrc
        )

    set_source_Files_properties(
        ${CMAKE_CURRENT_BINARY_DIR}/bundle/English.lproj/InfoPlist.strings
        PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/English.lproj")

    patch_xcode_plugin( "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.xcodeproj/project.pbxproj" "${PROJECT_NAME}" )
    patch_xcode_plugin( "${CMAKE_BINARY_DIR}/FireBreath.xcodeproj/project.pbxproj" "${PROJECT_NAME}" )

ENDMACRO(add_mac_plugin)

