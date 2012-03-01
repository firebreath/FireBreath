#/**********************************************************\ 
#Original Author: Richard Bateman (taxilian)
#
#Created:    Nov 20, 2009
#License:    Dual license model; choose one of two:
#            New BSD License
#            http://www.opensource.org/licenses/bsd-license.php
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#            
#Copyright 2009 PacketPass, Inc and the Firebreath development team
#\**********************************************************/

# Note that PluginConfig.cmake should have been included before this file!
# Also note that this file will be included once per plugin project and is
#   intended to assist with cleaning up configuration values, generating
#   needed files, etc.

get_filename_component(FB_TEMPLATE_DIR "${FB_ROOT_DIR}/gen_templates" ABSOLUTE)
get_filename_component(FB_TEMPLATE_DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/projects/${PLUGIN_NAME}/gen" ABSOLUTE)
message("Generating plugin configuration files in ${FB_TEMPLATE_DEST_DIR}")

# By this point we should have added any firebreath libraries that we want, so
# FBLIB_DEFINITONS should be filled out
add_definitions(${FBLIB_DEFINITIONS})

# Generally needed include directories for each plugin
set(PLUGIN_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FB_GECKOSDK_SOURCE_DIR}
    ${FB_INCLUDE_DIRS}
    ${FB_NPAPICORE_SOURCE_DIR}
    ${FB_ACTIVEXCORE_SOURCE_DIR}
    ${FB_SCRIPTINGCORE_SOURCE_DIR}
    ${FB_PLUGINAUTO_SOURCE_DIR}/${FB_PLATFORM_NAME}
    ${FB_PLUGINAUTO_SOURCE_DIR}
    ${FB_PLUGINCORE_SOURCE_DIR}
    ${FB_TEMPLATE_DEST_DIR}
    ${Boost_INCLUDE_DIRS}
    ${FBLIB_INCLUDE_DIRS}
    ${ATL_INCLUDE_DIRS}
    ${FB_CONFIG_DIR}
    )

if (NOT FBMAC_USE_CARBON AND NOT FBMAC_USE_COCOA)
    # As of Safari 5.1 we have to choose one even if we don't draw
    set(FBMAC_USE_COCOA 1)
endif()
if (NOT FBMAC_USE_INVALIDATINGCOREANIMATION
        AND NOT FBMAC_USE_COREANIMATION
        AND NOT FBMAC_USE_COREGRAPHICS
        AND NOT FBMAC_USE_QUICKDRAW)
    set(FBMAC_USE_COREGRAPHICS 1)
endif()

if (NOT FBMAC_USE_CARBON AND FBMAC_USE_QUICKDRAW)
    message("!! You can't use QuickDraw without Carbon; disabling QuickDraw (it's deprecated anyway)")
    if (NOT FBMAC_USE_INVALIDATINGCOREANIMATION
            AND NOT FBMAC_USE_COREANIMATION
            AND NOT FBMAC_USE_COREGRAPHICS)
        set(FBMAC_USE_COREGRAPHICS 1)
    endif()
endif()

# Clean up PluginConfig values as needed
# set up the mimetype strings
string(REPLACE ";" "|" FBMIMETYPE_LIST "${FBSTRING_MIMEType}")
string(REGEX REPLACE "\\.dll$" "" FBSTRING_PluginFileName "${FBSTRING_PluginFileName}")
if (FB_ATLREG_MACHINEWIDE)
    set(REGKEY_ROOT "HKLM")
    set(FB_WIX_INSTALL_LOCATION "ProgramFilesFolder")
else()
    set(REGKEY_ROOT "HKCU")
    set(FB_WIX_INSTALL_LOCATION "AppDataFolder")
endif()

set(FB_VERSION_SPLIT ${FBSTRING_PLUGIN_VERSION})
string(REPLACE "." ";" FB_VERSION_SPLIT ${FB_VERSION_SPLIT})
LIST(LENGTH FB_VERSION_SPLIT _LEN)

list(GET FB_VERSION_SPLIT 0 FBSTRING_VERSION_MAJOR)
if (_LEN GREATER 1)
    list(GET FB_VERSION_SPLIT 1 FBSTRING_VERSION_MINOR)
else()
    set(FBSTRING_VERSION_MINOR 0)
endif()

if (_LEN GREATER 2)
    list(GET FB_VERSION_SPLIT 2 FBSTRING_VERSION_PATCH)
else()
    set(FBSTRING_VERSION_PATCH 0)
endif()
if (_LEN GREATER 3)
    list(GET FB_VERSION_SPLIT 3 FBSTRING_VERSION_BUILD)
else()
    set(FBSTRING_VERSION_BUILD 0)
endif()

# fallback to define FBSTRING_PluginDescription for templates if the
# user hasn't done this in PluginConfig.cmake
if (NOT FBSTRING_PluginDescription)
    set(FBSTRING_PluginDescription ${FBSTRING_FileDescription})
endif()

# configure default generated files
# TODO: Fix this to not need configure_template; it is suboptimal
file(GLOB TEMPLATELIST ${FB_TEMPLATE_DIR}/[^.]*)
foreach(TemplateFile ${TEMPLATELIST})
    get_filename_component(CURFILE ${TemplateFile} NAME)
    get_filename_component(CUREXT ${TemplateFile} EXT)
    if (CUREXT STREQUAL ".h" OR CUREXT STREQUAL ".hpp" OR CUREXT STREQUAL ".inc")
        set (CUR_DEST ${FB_TEMPLATE_DEST_DIR}/global)
    else()
        set (CUR_DEST ${FB_TEMPLATE_DEST_DIR})
    endif()
    if (EXISTS ${FB_CURRENT_PLUGIN_DIR}/${CURFILE})
        if (VERBOSE)
            message("Configuring ${CURFILE} from project source dir")
        endif()
        configure_template(${FB_CURRENT_PLUGIN_DIR}/${CURFILE} ${CUR_DEST}/${CURFILE})
    else()
        if (VERBOSE)
            message("Configuring ${CURFILE}")
        endif()
        configure_template(${FB_TEMPLATE_DIR}/${CURFILE} ${CUR_DEST}/${CURFILE})
    endif()
endforeach()

# configure project-specific generated files
file(GLOB TEMPLATELIST ${FB_CURRENT_PLUGIN_DIR}/templates/[^.]*)
foreach(TemplateFile ${TEMPLATELIST})
    get_filename_component(CURFILE ${TemplateFile} NAME)
    message("Configuring ${CURFILE}")
    configure_template(${FB_CURRENT_PLUGIN_DIR}/templates/${CURFILE} ${FB_TEMPLATE_DEST_DIR}/../templates/${CURFILE})
endforeach()

# Repititions in the following are intentional to fix linking errors due to
# cyclic references on Linux. Don't change without testing on Linux!
set(PLUGIN_INTERNAL_DEPS
    PluginCore
    ${PLUGIN_PREFIX}_PluginAuto
    NpapiCore
    ScriptingCore
    PluginCore
    ${Boost_LIBRARIES}
    ${FBLIB_LIBRARIES}
    )

file (GLOB GENERATED
    ${FB_TEMPLATE_DEST_DIR}/[^.]*
    ${FB_TEMPLATE_DEST_DIR}/global/[^.]*
    )

file (GLOB WIN_GENERATED
    ${FB_TEMPLATE_DEST_DIR}/[^.]*.rgs
    ${FB_TEMPLATE_DEST_DIR}/[^.]*.def
    ${FB_TEMPLATE_DEST_DIR}/[^.]*.rc
    )
SOURCE_GROUP(Generated FILES ${GENERATED})

if (WIN32)
    set (PLUGIN_INTERNAL_DEPS
        ActiveXCore
        ${PLUGIN_INTERNAL_DEPS}
        ${ATL_LIBRARY}
        psapi
        Wininet
        )
    file (GLOB IDL_FILES
        ${FB_TEMPLATE_DEST_DIR}/*.idl)
    list(REMOVE_ITEM GENERATED ${IDL_FILES})
else()
    set_source_files_properties(${WIN_GENERATED}
        PROPERTIES
            HEADER_FILE_ONLY 1
        )
endif(WIN32)

if (APPLE)
    if (FBMAC_USE_CARBON)
        find_library(CARBON_FRAMEWORK Carbon) 
        set (PLUGIN_INTERNAL_DEPS
            ${CARBON_FRAMEWORK}
            ${PLUGIN_INTERNAL_DEPS})
    endif (FBMAC_USE_CARBON)

    #if (FBMAC_USE_COCOA)
    # In order to support async thread calls (needed for all events and other things)
    # we always need cocoa
    find_library(COCOA_FRAMEWORK Cocoa)
    set (PLUGIN_INTERNAL_DEPS
        ${COCOA_FRAMEWORK}
        ${PLUGIN_INTERNAL_DEPS})
    #endif (FBMAC_USE_COCOA)
    find_library(SYSCONFIG_FRAMEWORK SystemConfiguration)
    set (PLUGIN_INTERNAL_DEPS
        ${SYSCONFIG_FRAMEWORK}
        ${PLUGIN_INTERNAL_DEPS})

    if (FBMAC_USE_COREANIMATION OR FBMAC_USE_COREGRAPHICS OR FBMAC_USE_INVALIDATINGCOREANIMATION)
        find_library(FOUNDATION_FRAMEWORK Foundation)
        find_library(APPLICATIONSERVICES_FRAMEWORK ApplicationServices)
        set (PLUGIN_INTERNAL_DEPS
            ${FOUNDATION_FRAMEWORK}
            ${APPLICATIONSERVICES_FRAMEWORK}
            ${PLUGIN_INTERNAL_DEPS})
    endif (FBMAC_USE_COREANIMATION OR FBMAC_USE_COREGRAPHICS OR FBMAC_USE_INVALIDATINGCOREANIMATION)
   
    if (FBMAC_USE_COREANIMATION OR FBMAC_USE_INVALIDATINGCOREANIMATION)
        find_library(QUARTZCORE_FRAMEWORK QuartzCore)
        set (PLUGIN_INTERNAL_DEPS
            ${QUARTZCORE_FRAMEWORK}
            ${PLUGIN_INTERNAL_DEPS})
    endif (FBMAC_USE_COREANIMATION OR FBMAC_USE_INVALIDATINGCOREANIMATION)
endif (APPLE)

