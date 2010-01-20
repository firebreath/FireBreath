#/**********************************************************\ 
#Original Author: Richard Bateman (taxilian)
#
#Created:    Nov 20, 2009
#License:    Dual license model; choose one of two:
#            Eclipse Public License - Version 1.0
#            http://www.eclipse.org/legal/epl-v10.html
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#            
#Copyright 2009 PacketPass, Inc and the Firebreath development team
#\**********************************************************/

    
if (WIN32)
	set(CMAKE_C_FLAGS                            "/DWIN32 /W3 /wd4996 /nologo /D UNICODE /D _UNICODE /D _WINDOWS")
	set(CMAKE_CXX_FLAGS                          "/DWIN32 /W3 /wd4996 /nologo /EHsc /wd4290 /D UNICODE /D _UNICODE /D _WINDOWS")
	set(CMAKE_C_FLAGS_RELEASE                    "/MT /O1 /DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE                  "/MT /O1 /DNDEBUG")
	set(CMAKE_C_FLAGS_DEBUG                      "/MTd /Od /DDEBUG /D_DEBUG /ZI /RTC1 /Gm")
	set(CMAKE_CXX_FLAGS_DEBUG                    "/MTd /Od /DDEBUG /D_DEBUG /ZI /RTC1 /Gm")
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG
	    "${CMAKE_EXE_LINKER_FLAGS_DEBUG}         ")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE
	    "${CMAKE_EXE_LINKER_FLAGS_RELEASE}       ")
	set(CMAKE_SHARED_LINKER_FLAGS_DEBUG
	    "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}      /SUBSYSTEM:WINDOWS /MACHINE:X86")
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE
	    "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}    /SUBSYSTEM:WINDOWS /MACHINE:X86 /OPT:REF /OPT:ICF")
endif()

if (APPLE)
    # We define preprocessor flags here in addition to other flags
    # because it is the most convenient place to put them to apply
    # to all targets
    set (CMAKE_C_FLAGS "-DXP_MACOSX=1")
    set (CMAKE_CXX_FLAGS "-DXP_MACOSX=1")
endif()

if(UNIX AND NOT APPLE)
    # On Mac OS X UNIX is also defined
    # XP_UNIX is used by the gecko sdk
    set (CMAKE_C_FLAGS "-fPIC -DXP_UNIX=1")
    set (CMAKE_CXX_FLAGS "-fPIC -DXP_UNIX=1")
endif()

