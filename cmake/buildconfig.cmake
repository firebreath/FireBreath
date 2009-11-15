
    
if (WIN32)
	set(CMAKE_C_FLAGS                            "/DWIN32 /W3 /wd4996 /nologo /D UNICODE /D _UNICODE")
	set(CMAKE_CXX_FLAGS                          "/DWIN32 /W3 /wd4996 /nologo /EHsc /wd4290 /D UNICODE /D _UNICODE")
	set(CMAKE_C_FLAGS_RELEASE                    "/MT /O1 /DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE                  "/MT /O1 /DNDEBUG")
	set(CMAKE_C_FLAGS_DEBUG                      "/MTd /Od /DDEBUG /D_DEBUG /ZI /RTC1")
	set(CMAKE_CXX_FLAGS_DEBUG                    "/MTd /Od /DDEBUG /D_DEBUG /ZI /RTC1")
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG
	    "${CMAKE_EXE_LINKER_FLAGS_DEBUG}         /NODEFAULTLIB:libc,libcd,msvcrt,msvcrtd")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE
	    "${CMAKE_EXE_LINKER_FLAGS_RELEASE}       /NODEFAULTLIB:libc,libcd,msvcrt,msvcrtd")
	set(CMAKE_SHARED_LINKER_FLAGS_DEBUG
	    "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}      /NODEFAULTLIB:libc,libcd,msvcrt,msvcrtd")
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE
	    "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}    /NODEFAULTLIB:libc,libcd,msvcrt,msvcrtd")
endif()

