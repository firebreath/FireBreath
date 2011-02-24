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
    configure_template(${FB_CURRENT_PLUGIN_DIR}/templates/${CURFILE} ${TEMPLATE_DEST_DIR}/${CURFILE})
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

if (WIN32)
    set (PLUGIN_INTERNAL_DEPS
        ActiveXCore
        ${PLUGIN_INTERNAL_DEPS}
        ${ATL_LIBRARY}
        psapi
        )

    file (GLOB GENERATED
        ${FB_TEMPLATE_DEST_DIR}/[^.]*.rgs
        ${FB_TEMPLATE_DEST_DIR}/[^.]*.def
        ${FB_TEMPLATE_DEST_DIR}/[^.]*.rc
    )
    SOURCE_GROUP(Generated FILES ${GENERATED})

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

    if (FBMAC_USE_COREANIMATION OR FBMAC_USE_COREGRAPHICS)
        find_library(FOUNDATION_FRAMEWORK Foundation)
        find_library(APPLICATIONSERVICES_FRAMEWORK ApplicationServices)
        set (PLUGIN_INTERNAL_DEPS
            ${FOUNDATION_FRAMEWORK}
            ${APPLICATIONSERVICES_FRAMEWORK}
            ${PLUGIN_INTERNAL_DEPS})
    endif (FBMAC_USE_COREANIMATION OR FBMAC_USE_COREGRAPHICS)
   
    if (FBMAC_USE_COREANIMATION)
        find_library(QUARTZCORE_FRAMEWORK QuartzCore)
        set (PLUGIN_INTERNAL_DEPS
            ${QUARTZCORE_FRAMEWORK}
            ${PLUGIN_INTERNAL_DEPS})
    endif (FBMAC_USE_COREANIMATION)
endif (APPLE)

