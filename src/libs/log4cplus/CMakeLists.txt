project (log4cplus)
cmake_minimum_required (VERSION 2.8.0)

# with firebreath this target could get added mutliple times; protect against this
get_target_property(library_target_exists log4cplus TYPE)
if (library_target_exists)
    set(TARGET_ALREADY_EXISTS 1)
else()
    set(TARGET_ALREADY_EXISTS 0)
endif()

#clean up garbage file from old log4cplus version if needed
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/log4cplus/config/defines.hxx")
    message("!! Detected garbage file from old log4cplus version. Deleting ${CMAKE_CURRENT_SOURCE_DIR}/include/log4cplus/config/defines.hxx")
    file(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/include/log4cplus/config/defines.hxx")
endif()

set (log4cplus_version_major 1)
set (log4cplus_version_minor 1)
set (log4cplus_version_patch 0)
set (log4cplus_soversion 0)
set (log4cplus_postfix "")

find_package (Threads)
message (STATUS "Threads: ${CMAKE_THREAD_LIBS_INIT}")

set(BUILD_SHARED_LIBS FALSE CACHE BOOL "If TRUE, log4cplus is built as a shared library, otherwise as a static library")


if (MSVC)
  set (LOG4CPLUS_WORKING_LOCALE_DEFAULT ON)
else (MSVC)
  set (LOG4CPLUS_WORKING_LOCALE_DEFAULT OFF)
endif (MSVC)

option(LOG4CPLUS_WORKING_LOCALE "Define for compilers/standard libraries that support more than just the C locale."
  ${LOG4CPLUS_WORKING_LOCALE_DEFAULT})

option(LOG4CPLUS_WORKING_C_LOCALE
  "Define for compilers/standard libraries that have working C locale."
  OFF)

option (LOG4CPLUS_QT4 "Build with Qt4DebugAppender" OFF)

set(UNICODE ON)

option(WITH_ICONV "Use iconv() for char->wchar_t conversion."
  OFF)

#if (UNICODE)
  #set (log4cplus_postfix "${log4cplus_postfix}U")
#endif (UNICODE)

if (WITH_ICONV)
  set(LOG4CPLUS_WITH_ICONV 1)
endif ()

set (log4cplus "log4cplus${log4cplus_postfix}")

include(ConfigureChecks.cmake)

configure_file(include/log4cplus/config.h.cmake.in
  ${log4cplus_BINARY_DIR}/include/log4cplus/config.h @ONLY)
configure_file(include/log4cplus/config/defines.hxx.cmake.in
  ${log4cplus_BINARY_DIR}/include/log4cplus/config/defines.hxx @ONLY)

include_directories (${log4cplus_SOURCE_DIR}/include
                     ${log4cplus_BINARY_DIR}/include
                    )

if (NOT TARGET_ALREADY_EXISTS)
    add_subdirectory (src)
endif()

if (FIREBREATH)
    LIST(APPEND FBLIB_LIBRARIES log4cplus)
    LIST(APPEND FBLIB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include ${log4cplus_BINARY_DIR}/include)
    export_project_dependencies()
endif()
