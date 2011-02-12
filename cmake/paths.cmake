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

get_filename_component (PROJECTS_SOURCE_DIR "${PROJECTS_DIR}" ABSOLUTE)
set (PROJECTS_BINARY_DIR "${CMAKE_BINARY_DIR}/projects")

set (BOOST_SOURCE_DIR ${SOURCE_DIR}/3rdParty/boost)
set (BOOST_THREAD_SOURCE_DIR ${BOOST_SOURCE_DIR}/libs/thread)

set (GECKOSDK_SOURCE_DIR ${SOURCE_DIR}/3rdParty/gecko-sdk/includes)

set (ACTIVEXCORE_SOURCE_DIR "${SOURCE_DIR}/ActiveXCore")
set (ACTIVEXCORETEST_SOURCE_DIR "${TEST_DIR}/ActiveXCoreTest")

set (NPAPIHOST_SOURCE_DIR "${SOURCE_DIR}/NPAPIHost")

set (NPAPICORE_SOURCE_DIR "${SOURCE_DIR}/NpapiCore")
set (NPAPICORETEST_SOURCE_DIR "${TEST_DIR}/NpapiCoreTest")

set (SCRIPTINGCORE_SOURCE_DIR "${SOURCE_DIR}/ScriptingCore")
set (SCRIPTINGCORETEST_SOURCE_DIR "${TEST_DIR}/ScriptingCoreTest")

set (PLUGINCORE_SOURCE_DIR "${SOURCE_DIR}/PluginCore")

set (PLUGINAUTO_SOURCE_DIR "${SOURCE_DIR}/PluginAuto")

set (UNITTEST_FW_SOURCE_DIR "${TEST_DIR}/unittest-cpp/UnitTest++")

set(FBLIB_DIRS "${CMAKE_SOURCE_DIR}/src/libs")
