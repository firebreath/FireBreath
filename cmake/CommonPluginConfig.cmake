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

get_filename_component(FB_TEMPLATE_DIR "${FB_ROOT}/gen_templates" ABSOLUTE)
set(FB_TEMPLATE_DIR ${FB_TEMPLATE_DIR} CACHE INTERNAL "Directory to search for templates to generated files used by FireBreath")
set(FB_TEMPLATE_DEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/gen CACHE INTERNAL "Directory to search for templates to generated files used by FireBreath")
message("Generating plugin configuration files in ${FB_TEMPLATE_DEST_DIR}")

set(FB_COMMONPLUGINCONFIG 1 CACHE INTERNAL "Flag to indicate that CommonPluginConfig.cmake was included")

# By this point we should have added any firebreath libraries that we want, so
# FBLIB_DEFINITONS should be filled out
add_definitions(${FBLIB_DEFINITIONS})


# Generally needed include directories for each plugin
set(FB_PLUGIN_INCLUDE_DIRS
    ${FB_GECKOSDK_SOURCE_DIR}
    ${FB_INCLUDE_DIRS}
    ${ActiveXCore_SOURCE_DIR}
    ${ScriptingCore_SOURCE_DIR}
    ${FB_ROOT}/src/PluginAuto/${FB_PLATFORM_NAME}
    ${FB_ROOT}/src/ActiveXCore
    ${PluginCore_SOURCE_DIR}
    ${FB_TEMPLATE_DEST_DIR}
    ${Boost_INCLUDE_DIRS}
    ${FBLIB_INCLUDE_DIRS}
    ${ATL_INCLUDE_DIRS}
    ${FB_CONFIG_DIR}
    )

if (VERBOSE)

    message("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
    message("FB_GECKOSDK_SOURCE_DIR: ${FB_GECKOSDK_SOURCE_DIR}")
    message("FB_INCLUDE_DIRS: ${FB_INCLUDE_DIRS}")
    message("FB_ACTIVEXCORE_SOURCE_DIR: ${ActiveXCore_SOURCE_DIR}")
    message("FB_SCRIPTINGCORE_SOURCE_DIR: ${ScriptingCore_SOURCE_DIR}")
    message("FB_PLUGINAUTO_SOURCE_DIR: ${FB_ROOT}/src/PluginAuto/${FB_PLATFORM_NAME}")
    message("FB_PLUGINCORE_SOURCE_DIR: ${PluginCore_SOURCE_DIR}")
    message("FB_TEMPLATE_DEST_DIR: ${FB_TEMPLATE_DEST_DIR}")
    message("Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message("FBLIB_INCLUDE_DIRS: ${FBLIB_INCLUDE_DIRS}")
    message("ATL_INCLUDE_DIRS: ${ATL_INCLUDE_DIRS}")
    message("FB_CONFIG_DIR: ${FB_CONFIG_DIR}")

endif()

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
    set(FB_WIX_INSTALL_SCOPE "perMachine")
else()
    set(REGKEY_ROOT "HKCU")
    set(FB_WIX_INSTALL_LOCATION "AppDataFolder")
    set(FB_WIX_INSTALL_SCOPE "perUser")
endif()

if (PLUGIN_CRX_ALLOWED_ORIGINS)
    string(REPLACE ";" "~'~, ~'~" PLUGIN_CRX_ALLOWED_ORIGINS "${PLUGIN_CRX_ALLOWED_ORIGINS}")
    set(PLUGIN_CRX_ALLOWED_ORIGINS "~'~${PLUGIN_CRX_ALLOWED_ORIGINS}~'~")
else()
    set(PLUGIN_CRX_ALLOWED_ORIGINS "~'~chrome-extension://${PLUGIN_CRX_ID}/~'~")
endif()

if (PLUGIN_XPI_ALLOWED_EXTENSIONS)
    string(REPLACE ";" "~'~, ~'~" PLUGIN_XPI_ALLOWED_EXTENSIONS "${PLUGIN_XPI_ALLOWED_EXTENSIONS}")
    set(PLUGIN_XPI_ALLOWED_EXTENSIONS "~'~${PLUGIN_XPI_ALLOWED_EXTENSIONS}~'~")
else()
    set(PLUGIN_XPI_ALLOWED_EXTENSIONS "~'~${PLUGIN_MOZ_ID}~'~")
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
macro(firebreath_configure_templates TEMPLATE_DIR TEMPLATE_DEST HEADER_PREFIX OVERRIDE_DIR)
    file(GLOB TEMPLATELIST ${TEMPLATE_DIR}/[^.]*)
    set(NAMESHOST "@NAMESHOST@")
    foreach(TemplateFile ${TEMPLATELIST})
        get_filename_component(CURFILE ${TemplateFile} NAME)
        get_filename_component(CUREXT ${TemplateFile} EXT)
        if (CUREXT STREQUAL ".h" OR CUREXT STREQUAL ".hpp" OR CUREXT STREQUAL ".inc")
            set (HEADER_DETECTED 1)
        else()
            set (HEADER_DETECTED 0)
        endif()
        if (NOT "" STREQUAL "${HEADER_PREFIX}" AND HEADER_DETECTED)
            set (CUR_DEST ${TEMPLATE_DEST}/${HEADER_PREFIX})
        else()
            set (CUR_DEST ${TEMPLATE_DEST})
        endif()
        set (SOURCE_FILE "${TEMPLATE_DIR}/${CURFILE}")
        if (OVERRIDE_DIR AND EXISTS ${OVERRIDE_DIR}/${CURFILE})
            if (VERBOSE)
                message("Configuring ${CURFILE} from plugin-provided alternate source dir")
            endif()
            set (SOURCE_FILE "${OVERRIDE_DIR}/${CURFILE}")
        else()
            if (VERBOSE)
                message("Configuring ${CURFILE}")
            endif()
        endif()
        configure_template(${SOURCE_FILE} ${CUR_DEST}/${CURFILE})
    endforeach()
endmacro(firebreath_configure_templates)

macro(firebreath_generate_templates OVERRIDE_DIR)
    firebreath_configure_templates(${FB_TEMPLATE_DIR} ${FB_TEMPLATE_DEST_DIR} "global" ${OVERRIDE_DIR})
endmacro(firebreath_generate_templates)

# This command configures the templates normally found in ${FB_ROOT}/gen_templates
# to override one of the templates, copy it from there to the directory passed into
# this function
firebreath_generate_templates(${CMAKE_CURRENT_SOURCE_DIR})

# Repititions in the following are intentional to fix linking errors due to
# cyclic references on Linux. Don't change without testing on Linux!

if(UNIX AND NOT APPLE) 
    set(FB_PLUGIN_LIBRARIES 
        PluginCore 
        -Wl,--whole-archive 
        ${PLUGIN_PREFIX}_PluginAuto 
        -Wl,--no-whole-archive 
        NpapiCore 
        FireWyrm 
        ScriptingCore 
        PluginCore 
        ${Boost_LIBRARIES} 
        ${FBLIB_LIBRARIES} 
        )
else()
    set(FB_PLUGIN_LIBRARIES 
        PluginCore 
        ${PLUGIN_PREFIX}_PluginAuto 
        NpapiCore 
        FireWyrm 
        ScriptingCore 
        PluginCore 
        ${Boost_LIBRARIES} 
        ${FBLIB_LIBRARIES} 
        )
endif()

file (GLOB FB_PLUGIN_GENERATED_SOURCES
    ${FB_TEMPLATE_DEST_DIR}/[^.]*
    ${FB_TEMPLATE_DEST_DIR}/global/[^.]*
    )

file (GLOB FB_PLUGIN_GENERATED_WIN_SOURCES
    ${FB_TEMPLATE_DEST_DIR}/[^.]*.rgs
    ${FB_TEMPLATE_DEST_DIR}/[^.]*.def
    ${FB_TEMPLATE_DEST_DIR}/[^.]*.rc
    )
SOURCE_GROUP(Generated FILES ${FB_PLUGIN_GENERATED_SOURCES} ${FB_PLUGIN_GENERATED_WIN_SOURCES})

if (WIN32)
    set (FB_PLUGIN_LIBRARIES
        ActiveXCore
        ${FB_PLUGIN_LIBRARIES}
        ${ATL_LIBRARY}
        psapi
        Wininet
        )
    file (GLOB IDL_FILES
        ${FB_TEMPLATE_DEST_DIR}/*.idl)
    list(REMOVE_ITEM FB_PLUGIN_GENERATED_SOURCES "" ${IDL_FILES})
else()
    set_source_files_properties(${FB_PLUGIN_GENERATED_WIN_SOURCES}
        PROPERTIES
            HEADER_FILE_ONLY 1
        )
endif(WIN32)

if (APPLE)
    if (FBMAC_USE_CARBON)
        find_library(FRAMEWORK_CARBON Carbon) 
        set (FB_PLUGIN_LIBRARIES
            ${FRAMEWORK_CARBON}
            ${FB_PLUGIN_LIBRARIES})
    endif (FBMAC_USE_CARBON)

    # In order to support async thread calls (needed for all events and other things)
    # we always need cocoa
    find_library(FRAMEWORK_COCOA Cocoa)
    set (FB_PLUGIN_LIBRARIES
        ${FRAMEWORK_COCOA}
        ${FB_PLUGIN_LIBRARIES})

    find_library(FRAMEWORK_SYSCONFIG SystemConfiguration)
    set (FB_PLUGIN_LIBRARIES
        ${FRAMEWORK_SYSCONFIG}
        ${FB_PLUGIN_LIBRARIES})

    if (FBMAC_USE_COREANIMATION OR FBMAC_USE_COREGRAPHICS OR FBMAC_USE_INVALIDATINGCOREANIMATION)
        find_library(FRAMEWORK_FOUNDATION Foundation)
        find_library(FRAMEWORK_APPLICATIONSERVICES ApplicationServices)
        set (FB_PLUGIN_LIBRARIES
            ${FRAMEWORK_FOUNDATION}
            ${FRAMEWORK_APPLICATIONSERVICES}
            ${FB_PLUGIN_LIBRARIES})
    endif (FBMAC_USE_COREANIMATION OR FBMAC_USE_COREGRAPHICS OR FBMAC_USE_INVALIDATINGCOREANIMATION)

    if (FBMAC_USE_COREANIMATION OR FBMAC_USE_INVALIDATINGCOREANIMATION)
        find_library(FRAMEWORK_QUARTZCORE QuartzCore)
        set (FB_PLUGIN_LIBRARIES
            ${FRAMEWORK_QUARTZCORE}
            ${FB_PLUGIN_LIBRARIES})
    endif (FBMAC_USE_COREANIMATION OR FBMAC_USE_INVALIDATINGCOREANIMATION)
endif (APPLE)

