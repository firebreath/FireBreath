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
#Copyright 2009-2010 PacketPass, Inc and the Firebreath development team
#\**********************************************************/

    
if (WIN32)
    set(CMAKE_C_FLAGS                            "/DWIN32 /DFB_WIN=1 /DXP_WIN=1 /W3 /wd4996 /nologo /D UNICODE /D _UNICODE /D _WINDOWS")
    if (NOT WITH_SYSTEM_BOOST)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D BOOST_ALL_NO_LIB=1")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D BOOST_ALL_NO_LIB=1")
    endif()
    set(CMAKE_CXX_FLAGS                          "/DWIN32 /DFB_WIN=1 /DXP_WIN=1 /W3 /wd4996 /nologo /EHsc /wd4290 /D UNICODE /D _UNICODE /D _WINDOWS")
    set(CMAKE_C_FLAGS_RELEASE                    "/MT /Ox /DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE                  "/MT /Ox /DNDEBUG")
    set(CMAKE_C_FLAGS_DEBUG                      "/MTd /Od /DDEBUG /D_DEBUG /ZI /RTC1 /Gm")
    set(CMAKE_CXX_FLAGS_DEBUG                    "/MTd /Od /DDEBUG /D_DEBUG /ZI /RTC1 /Gm")
    set(CMAKE_C_FLAGS_MINSIZEREL                 "/MT /O1 /DNDEBUG")
    set(CMAKE_CXX_FLAGS_MINSIZEREL               "/MT /O1 /DNDEBUG")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO             "/MT /Ox /DNDEBUG /Zi")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO           "/MT /Ox /DNDEBUG /Zi")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG
        "${CMAKE_EXE_LINKER_FLAGS_DEBUG}         ")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE
        "${CMAKE_EXE_LINKER_FLAGS_RELEASE}       ")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG
        "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}      /SUBSYSTEM:WINDOWS")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE
        "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}    /SUBSYSTEM:WINDOWS /OPT:REF /OPT:ICF")

    set(LINK_FLAGS "/LIBPATH:\"${ATL_LIBRARY_DIR}\"")

    if (WITH_DYNAMIC_MSVC_RUNTIME)
        message(STATUS "Building with dynamic MSVC runtime")
        foreach(flag_var
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
            if(${flag_var} MATCHES "/MT")
                string(REGEX REPLACE "/MT" "/MD" ${flag_var} "${${flag_var}}")
            endif()
        endforeach(flag_var)
    endif()
endif()

# We define preprocessor flags here in addition to other flags
# because it is the most convenient place to put them to apply
# to all targets
if(UNIX)
    # XP_UNIX is used by the Gecko SDK
    set(gecko_defs "-DXP_UNIX")
    if(APPLE)
        # In addition, Gecko SDK on Mac OS X needs XP_MACOSX
        set(gecko_defs "${gecko_defs} -DXP_MACOSX")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFB_MACOSX=1")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFB_MACOSX=1")
    endif()

    if(NOT APPLE)
        # On 64 bit, -fPIC is required for compiling all objects which get linked
        # into a .so file.
        # CMake correctly defaults to -fPIC for shared libs, but static libs are
        # built without. Since we are linking static libs later into a shared lib
        # (as opposed to an executable), we need to build static libs with -fPIC
        # too.
        set(fPIC_flag "-fPIC")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFB_X11=1")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFB_X11=1")
    endif()

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${gecko_defs} ${fPIC_flag}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${gecko_defs} ${fPIC_flag}")
    if (NOT WITH_SYSTEM_BOOST)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBOOST_ALL_NO_LIB=1")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_ALL_NO_LIB=1")
    endif()

    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DNDEBUG")
endif()
