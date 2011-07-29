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

get_filename_component (FB_PROJECTS_DIR "${FB_PROJECTS_DIR}" ABSOLUTE)
set (FB_PROJECTS_BINARY_DIR "${CMAKE_BINARY_DIR}/projects")

set (FB_BOOST_SOURCE_DIR ${FB_SOURCE_DIR}/3rdParty/boost)
set (FB_GECKOSDK_SOURCE_DIR ${FB_SOURCE_DIR}/3rdParty/gecko-sdk/includes)
set (FB_UTF8_SOURCE_DIR ${FB_SOURCE_DIR}/3rdParty/utf8)

set (FB_ACTIVEXCORE_SOURCE_DIR "${FB_SOURCE_DIR}/ActiveXCore")
set (FB_ACTIVEXCORE_BUILD_DIR "${FB_BUILD_DIR}/ActiveXCore")
set (FB_ACTIVEXCORETEST_SOURCE_DIR "${FB_TEST_DIR}/ActiveXCoreTest")
set (FB_ACTIVEXCORETEST_BUILD_DIR "${FB_BUILD_DIR}/ActiveXCoreTest")

set (FB_NPAPIHOST_SOURCE_DIR "${FB_SOURCE_DIR}/NPAPIHost")
set (FB_NPAPIHOST_BUILD_DIR "${FB_BUILD_DIR}/NPAPIHost")

set (FB_NPAPICORE_SOURCE_DIR "${FB_SOURCE_DIR}/NpapiCore")
set (FB_NPAPICORE_BUILD_DIR "${FB_BUILD_DIR}/NpapiCore")
set (FB_NPAPICORETEST_SOURCE_DIR "${FB_TEST_DIR}/NpapiCoreTest")
set (FB_NPAPICORETEST_BUILD_DIR "${FB_BUILD_DIR}/NpapiCoreTest")

set (FB_SCRIPTINGCORE_SOURCE_DIR "${FB_SOURCE_DIR}/ScriptingCore")
set (FB_SCRIPTINGCORE_BUILD_DIR "${FB_BUILD_DIR}/ScriptingCore")
set (FB_SCRIPTINGCORETEST_SOURCE_DIR "${FB_TEST_DIR}/ScriptingCoreTest")
set (FB_SCRIPTINGCORETEST_BUILD_DIR "${FB_BUILD_DIR}/ScriptingCoreTest")

set (FB_PLUGINCORE_SOURCE_DIR "${FB_SOURCE_DIR}/PluginCore")
set (FB_PLUGINCORE_BUILD_DIR "${FB_BUILD_DIR}/PluginCore")

set (FB_PLUGINAUTO_SOURCE_DIR "${FB_SOURCE_DIR}/PluginAuto")

set (FB_UNITTEST_FW_SOURCE_DIR "${FB_TEST_DIR}/unittest-cpp/UnitTest++")
set (FB_UNITTEST_FW_BUILD_DIR "${FB_BUILD_DIR}/unittest-cpp/UnitTest++")

set (FB_WEBKITCORE_SOURCE_DIR "${FB_SOURCE_DIR}/WebKitCore")
set (FB_WEBKITCORE_BUILD_DIR "${FB_BUILD_DIR}/WebKitCore")

set(FBLIB_DIRS "${FB_SOURCE_DIR}/libs")
