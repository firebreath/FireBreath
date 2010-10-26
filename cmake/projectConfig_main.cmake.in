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

get_filename_component (GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}" ABSOLUTE)
configure_file(PluginConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/PluginConfig.cmake COPYONLY)

include(${CMAKE_DIR}/PluginConfigDefaults.cmake)
include(PluginConfig.cmake)

get_filename_component(TEMPLATE_DIR "${FB_ROOT_DIR}/gen_templates" ABSOLUTE)
get_filename_component(TEMPLATE_DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/gen" ABSOLUTE)
message("Generating plugin configuration files in ${TEMPLATE_DEST_DIR}")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GECKOSDK_SOURCE_DIR}
    ${GTK_INCLUDE_DIRS}
    ${ACTIVEXPLUGIN_SOURCE_DIR}
    ${NPAPIPLUGIN_SOURCE_DIR}
    ${WINDOWSCOMMON_SOURCE_DIR}
    ${SCRIPTINGCORE_SOURCE_DIR}
    ${PLUGINCOMMON_SOURCE_DIR}
    ${PLUGINCORE_SOURCE_DIR}
    ${PLUGINWINDOW_SOURCE_DIR}
    ${GEN_DIR}/gen
    ${Boost_INCLUDE_DIRS}
    ${FBLIB_INCLUDE_DIRS}
    )

# configure default generated files
file(GLOB TEMPLATELIST ${TEMPLATE_DIR}/[^.]*)
foreach(TemplateFile ${TEMPLATELIST})
    get_filename_component(CURFILE ${TemplateFile} NAME)
    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${CURFILE})
        if (VERBOSE)
            message("Configuring ${CURFILE} from project source dir")
        endif()
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${CURFILE} ${TEMPLATE_DEST_DIR}/${CURFILE})
    else()
        if (VERBOSE)
            message("Configuring ${CURFILE}")
        endif()
        configure_file(${TEMPLATE_DIR}/${CURFILE} ${TEMPLATE_DEST_DIR}/${CURFILE})
    endif()
endforeach()

# configure project-specific generated files
file(GLOB TEMPLATELIST ${CMAKE_CURRENT_SOURCE_DIR}/templates/[^.]*)
foreach(TemplateFile ${TEMPLATELIST})
    get_filename_component(CURFILE ${TemplateFile} NAME)
    message("Configuring ${CURFILE}")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/templates/${CURFILE} ${TEMPLATE_DEST_DIR}/${CURFILE})
endforeach()

# Repititions in the following are intentional to fix linking errors due to
# cyclic references on Linux. Don't change without testing on Linux!
set(PLUGIN_INTERNAL_DEPS
    ${Boost_LIBRARIES}
    PluginCore
    ${PLUGIN_PREFIX}_NpapiPlugin
    ${PLUGIN_PREFIX}_PluginWindow
    PluginCore
    ScriptingCore
    ${PLUGIN_PREFIX}_PluginCommon
    ${Boost_LIBRARIES}
    ${FBLIB_LIBRARIES}
    )

if (WIN32)
    set (PLUGIN_INTERNAL_DEPS
        ${PLUGIN_PREFIX}_ActiveXPlugin
        ${PLUGIN_INTERNAL_DEPS}
        )

    file (GLOB GENERATED RELATIVE ${CMAKE_CURRENT_BINARY_DIR}
        ${GEN_DIR}/gen/[^.]*.rgs
        ${GEN_DIR}/gen/[^.]*.def
        ${GEN_DIR}/gen/[^.]*.rc
    )

endif(WIN32)

if (APPLE)
    if (FBMAC_USE_CARBON)
        find_library(CARBON_FRAMEWORK Carbon) 
        set (PLUGIN_INTERNAL_DEPS
            ${CARBON_FRAMEWORK}
            ${PLUGIN_INTERNAL_DEPS})
    endif (FBMAC_USE_CARBON)

    if (FBMAC_USE_COCOA)
        find_library(COCOA_FRAMEWORK Cocoa)
        set (PLUGIN_INTERNAL_DEPS
            ${COCOA_FRAMEWORK}
            ${PLUGIN_INTERNAL_DEPS})
    endif (FBMAC_USE_COCOA)

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

