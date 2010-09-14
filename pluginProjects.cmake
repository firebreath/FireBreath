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

if (WIN32)  # ActiveX projects
  add_subdirectory(${ACTIVEXPLUGIN_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/ActiveXPlugin)
  add_subdirectory(${ACTIVEXPLUGINTEST_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/ActiveXPluginTest)
endif(WIN32)

add_subdirectory(${NPAPIPLUGIN_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/NpapiPlugin)
add_subdirectory(${NPAPIPLUGINTEST_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/NpapiPluginTest)
add_subdirectory(${PLUGINWINDOW_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/PluginWindow)
if (NOT WITH_SYSTEM_BOOST)
    add_subdirectory(${BOOST_THREAD_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/boost_thread)
endif()

