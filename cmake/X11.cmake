#/**********************************************************\ 
#Original Author: Richard Prescott (rdprescott)
#  based in Win.cmake of the same directory
#
#Created:    Jan 19, 2010
#License:    Dual license model; choose one of two:
#            Eclipse Public License - Version 1.0
#            http://www.eclipse.org/legal/epl-v10.html
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#            
#Copyright 2010 Richard Prescott <rdprescott@gmail.com>
#\**********************************************************/

include(${CMAKE_DIR}/FindPkgConfig.cmake)

# not even optional
set(BUILD_SHARED_LIBS YES)

set(CMAKE_SHARED_LIBRARY_PREFIX "")

# Control symbol visibility and error handling with linker flags.
# -Wl,--discard-all     - discard all local symbols
# -Wl,-z,defs           - report unresolved symbol references
# -Wl,-Bsymbolic        - do not allow browser globals to override plugins
# -Wl,--version-script= - List symbols to be exported in the plugin DSO

set(NPAPI_LINK_FLAGS "-Wl,--discard-all -Wl,-Bsymbolic -Wl,-z,defs -Wl,--version-script=${FB_ROOT_DIR}/gen_templates/version_script.txt")

# if (NOT GTK_INCLUDE_DIRS)
#     pkg_check_modules(GTK REQUIRED gtk+-2.0)
#     set (GTK_INCLUDE_DIRS ${GTK_INCLUDE_DIRS} CACHE INTERNAL "GTK include dirs")
# endif()

function (add_rpm_package PROJNAME )

endfunction(add_rpm_package)

function (add_rpm_package PROJNAME )

endfunction(add_rpm_package)

function (add_deb_package PROJNAME )

endfunction(add_deb_package)

