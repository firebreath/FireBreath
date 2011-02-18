#/**********************************************************\ 
#Original Author: Richard Prescott (rdprescott)
#  based in Win.cmake of the same directory
#
#Created:    Jan 19, 2010
#License:    Dual license model; choose one of two:
#            New BSD License
#            http://www.opensource.org/licenses/bsd-license.php
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#            
#Copyright 2010 Richard Prescott <rdprescott@gmail.com>
#\**********************************************************/

find_package(PkgConfig)

# not even optional
set(BUILD_SHARED_LIBS YES)

# Control symbol visibility and error handling with linker flags.
# -Wl,--discard-all     - discard all local symbols
# -Wl,-z,defs           - report unresolved symbol references
# -Wl,-Bsymbolic        - do not allow browser globals to override plugins
# -Wl,--version-script= - List symbols to be exported in the plugin DSO

# This long line is ugly, but breaking it up to multiple lines will 
# break on cmake 2.6. LD_FLAGS will get separated by semi-colon 
# which is not gcc compatible
set(NPAPI_LINK_FLAGS "-Wl,--discard-all -Wl,-Bsymbolic -Wl,-z,defs -Wl,--version-script=${FB_ROOT_DIR}/gen_templates/version_script.txt")

# GTK is required for XEmbed to work
if (NOT FB_GUI_DISABLED)
    if (NOT GTK_INCLUDE_DIRS)
        pkg_check_modules(GTK REQUIRED gtk+-2.0)
        set (GTK_INCLUDE_DIRS ${GTK_INCLUDE_DIRS} CACHE INTERNAL "GTK include dirs")
        set (GTK_LIBRARIES ${GTK_LIBRARIES} CACHE INTERNAL "GTK include dirs")
        set (GTK_LIBRARY_DIRS ${GTK_LIBRARY_DIRS} CACHE INTERNAL "GTK include dirs")
        set (GTK_LDFLAGS ${GTK_LDFLAGS} CACHE INTERNAL "GTK include dirs")
    endif()
    set (FB_INCLUDE_DIRS ${FB_INCLUDE_DIRS} ${GTK_INCLUDE_DIRS})
else()
    set (GTK_INCLUDE_DIRS "")
    set (GTK_LIBRARIES "")
    set (GTK_LDFLAGS "")
endif()

MACRO(add_x11_plugin PROJNAME INSOURCES)
    add_definitions(
        -D"FB_X11=1"
    )

    set(SOURCES
        ${${INSOURCES}}
    )

    add_library(${PROJNAME} SHARED ${SOURCES})

    set_target_properties ("${PROJNAME}" PROPERTIES
        OUTPUT_NAME np${PLUGIN_NAME}
        PROJECT_LABEL "${PROJNAME}"
        LINK_FLAGS "${NPAPI_LINK_FLAGS}"
        PREFIX ""
        RUNTIME_OUTPUT_DIRECTORY "${FB_BIN_DIR}/${PLUGIN_NAME}"
        LIBRARY_OUTPUT_DIRECTORY "${FB_BIN_DIR}/${PLUGIN_NAME}"
    )
ENDMACRO(add_x11_plugin)

function (add_rpm_package PROJNAME )

endfunction(add_rpm_package)

function (add_rpm_package PROJNAME )

endfunction(add_rpm_package)

function (add_deb_package PROJNAME )

endfunction(add_deb_package)

set (GUI_INCLUDE_DIRS ${GTK_INCLUDE_DIRS} CACHE INTERNAL "GTK include dirs")
set (GUI_LIBRARIES ${GTK_LIBRARIES} CACHE INTERNAL "GTK include dirs")
set (GUI_LIBRARY_DIRS ${GTK_LIBRARY_DIRS} CACHE INTERNAL "GTK include dirs")
set (GUI_LDFLAGS ${GTK_LDFLAGS} CACHE INTERNAL "GTK include dirs")
