
get_filename_component (CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component (FB_ROOT_DIR "${CMAKE_DIR}/.." ABSOLUTE)
get_filename_component (SOURCE_DIR "${CMAKE_DIR}/../src" ABSOLUTE)
get_filename_component (BUILD_DIR "${CMAKE_DIR}/../build" ABSOLUTE)

set (PROJECT_ROOT "${BUILD_DIR}/projects")
set (BIN_DIR "${BUILD_DIR}/bin")

if (NOT EXISTS ${BUILD_DIR})
   file (MAKE_DIRECTORY ${BUILD_DIR})
endif()

if (WIN32)
	set(CMAKE_C_FLAGS           "/DWIN32 /W3 /wd4996 /nologo")
	set(CMAKE_CXX_FLAGS         "/DWIN32 /W3 /wd4996 /nologo /EHsc /wd4290")
	set(CMAKE_C_FLAGS_RELEASE          "/MT /O1 /DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE        "/MT /O1 /DNDEBUG")
	set(CMAKE_C_FLAGS_DEBUG            "/MTd /Od /DDEBUG /D_DEBUG /ZI /RTC1")
	set(CMAKE_CXX_FLAGS_DEBUG          "/MTd /Od /DDEBUG /D_DEBUG /ZI /RTC1")
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG
	    "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:libc,libcd,msvcrt,msvcrtd")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE
	    "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /NODEFAULTLIB:libc,libcd,msvcrt,msvcrtd")
	set(CMAKE_SHARED_LINKER_FLAGS_DEBUG
	    "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:libc,libcd,msvcrt,msvcrtd")
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE
	    "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /NODEFAULTLIB:libc,libcd,msvcrt,msvcrtd")
	set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO    "/debugtype:cv /pdb:none")
	set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "/debugtype:cv /pdb:none")
endif()

set (FIREBREATH_SOURCE_DIR ${FB_ROOT_DIR})
set (FIREBREATH_BINARY_DIR ${BUILD_DIR})

set (BOOST_SOURCE_DIR ${SOURCE_DIR}/3rdParty/boost)
set (GECKOSDK_SOURCE_DIR ${SOURCE_DIR}/3rdParty/gecko-sdk/win/includes)

set (FIREBREATHWIN_SOURCE_DIR "${FB_ROOT_DIR}/FireBreathWin")
set (FIREBREATHWIN_BINARY_DIR "${PROJECT_ROOT}/FireBreathWin")

set (ACTIVEXPLUGIN_SOURCE_DIR "${SOURCE_DIR}/ActiveXPlugin")
set (ACTIVEXPLUGINTEST_SOURCE_DIR "${SOURCE_DIR}/ActiveXPluginTest")

set (NPAPIHOST_SOURCE_DIR "${SOURCE_DIR}/NPAPIHost")

set (NPAPIPLUGIN_SOURCE_DIR "${SOURCE_DIR}/NpapiPlugin")
set (NPAPIPLUGINTEST_SOURCE_DIR "${SOURCE_DIR}/NpapiPluginTest")

set (SCRIPTINGCORE_SOURCE_DIR "${SOURCE_DIR}/ScriptingCore")
set (SCRIPTINGCORETEST_SOURCE_DIR "${SOURCE_DIR}/ScriptingCoreTest")

set (WINDOWSCOMMON_SOURCE_DIR "${SOURCE_DIR}/WindowsCommon")

set (UNITTEST_FW_SOURCE_DIR "${SOURCE_DIR}/unittest-cpp/UnitTest++")
set (UNITTEST_FW_BINARY_DIR "${PROJECT_ROOT}/UnitTest++")


