project (log4cplus CXX C)
cmake_minimum_required (VERSION 2.6)
set (CMAKE_VERBOSE_MAKEFILE on)

# with firebreath this target could get added mutliple times; protect against this
get_target_property(library_target_exists log4cplus TYPE)
if (library_target_exists)
    set(TARGET_ALREADY_EXISTS 1)
else()
    set(TARGET_ALREADY_EXISTS 0)
endif()

set (log4cplus_version_major 1)
set (log4cplus_version_minor 0)
set (log4cplus_version_patch 4)
set (log4cplus_soversion 4)
set (log4cplus_postfix "")

find_package (Threads)
if (VERBOSE)
    message (STATUS "Threads: ${CMAKE_THREAD_LIBS_INIT}")
endif()

set (log4cplus_headers
  include/log4cplus/appender.h
  include/log4cplus/config/macosx.h
  include/log4cplus/config/win32.h
  include/log4cplus/config.hxx
  include/log4cplus/configurator.h
  include/log4cplus/consoleappender.h
  include/log4cplus/fileappender.h
  include/log4cplus/fstreams.h
  include/log4cplus/helpers/appenderattachableimpl.h
  include/log4cplus/helpers/loglog.h
  include/log4cplus/helpers/logloguser.h
  include/log4cplus/helpers/pointer.h
  include/log4cplus/helpers/property.h
  include/log4cplus/helpers/sleep.h
  include/log4cplus/helpers/socket.h
  include/log4cplus/helpers/socketbuffer.h
  include/log4cplus/helpers/stringhelper.h
  include/log4cplus/helpers/syncprims.h
  include/log4cplus/helpers/thread-config.h
  include/log4cplus/helpers/threads.h
  include/log4cplus/helpers/timehelper.h
  include/log4cplus/hierarchy.h
  include/log4cplus/hierarchylocker.h
  include/log4cplus/layout.h
  include/log4cplus/logger.h
  include/log4cplus/loggingmacros.h
  include/log4cplus/loglevel.h
  include/log4cplus/ndc.h
  include/log4cplus/nteventlogappender.h
  include/log4cplus/nullappender.h
  include/log4cplus/socketappender.h
  include/log4cplus/spi/appenderattachable.h
  include/log4cplus/spi/factory.h
  include/log4cplus/spi/filter.h
  include/log4cplus/spi/loggerfactory.h
  include/log4cplus/spi/loggerimpl.h
  include/log4cplus/spi/loggingevent.h
  include/log4cplus/spi/objectregistry.h
  include/log4cplus/spi/rootlogger.h
  include/log4cplus/streams.h
  include/log4cplus/syslogappender.h
  include/log4cplus/tstring.h
  include/log4cplus/version.h
  include/log4cplus/win32debugappender.h
  include/log4cplus/win32consoleappender.h)


set (log4cplus_sources
  src/appender.cxx
  src/appenderattachableimpl.cxx
  src/configurator.cxx
  src/consoleappender.cxx
  src/factory.cxx
  src/fileappender.cxx
  src/filter.cxx
  src/global-init.cxx
  src/hierarchy.cxx
  src/hierarchylocker.cxx
  src/layout.cxx
  src/logger.cxx
  src/loggerimpl.cxx
  src/loggingevent.cxx
  src/loglevel.cxx
  src/loglog.cxx
  src/logloguser.cxx
  src/ndc.cxx
  src/nullappender.cxx
  src/objectregistry.cxx
  src/patternlayout.cxx
  src/pointer.cxx
  src/property.cxx
  src/rootlogger.cxx
  src/sleep.cxx
  src/socket.cxx
  src/socketappender.cxx
  src/socketbuffer.cxx
  src/stringhelper.cxx
  src/syncprims.cxx
  src/syslogappender.cxx
  src/threads.cxx
  src/timehelper.cxx
  src/version.cxx)

if (VERBOSE)
    message (STATUS "Type: ${UNIX}|${CYGWIN}|${WIN32}")
endif()

# Uncomment the following line to get locale aware string conversions.
#add_definitions (-DLOG4CPLUS_WORKING_LOCALE)

if (NOT TARGET_ALREADY_EXISTS)
    if (APPLE)
        include_directories(${CMAKE_OSX_SYSROOT}/usr/include)
        set(CMAKE_REQUIRED_INCLUDES ${CMAKE_OSX_SYSROOT}/usr/include)
    endif()

#    IF (CMAKE_XCODE_ATTRIBUTE_GCC_VERSION STRE
    if (APPLE AND CMAKE_XCODE_ATTRIBUTE_GCC_VERSION STREQUAL "4.0")
        set(LOG4CPLUS_HAVE_SYS_TYPES_H ON)
        set(LOG4CPLUS_HAVE_SYS_SOCKET_H ON)
        set(LOG4CPLUS_HAVE_SYS_TIME_H ON)
        set(LOG4CPLUS_HAVE_SYS_TIMEB_H ON)
        set(LOG4CPLUS_HAVE_SYS_STAT_H ON)
        set(LOG4CPLUS_HAVE_SYSLOG_H ON)
        set(LOG4CPLUS_HAVE_NETINET_IN_H ON)
        set(LOG4CPLUS_HAVE_NETDB_H ON)
        set(LOG4CPLUS_HAVE_UNISTD_H ON)
        set(LOG4CPLUS_HAVE_STDIO_H ON)
        set(LOG4CPLUS_HAVE_WCHAR_H ON)
        set(LOG4CPLUS_HAVE_STDARG_H ON)
        set(LOG4CPLUS_HAVE_TIME_H ON)
        set(LOG4CPLUS_HAVE_ERRNO_H ON)

        set(LOG4CPLUS_HAVE_GMTIME_R ON)
        set(LOG4CPLUS_HAVE_LOCALTIME_R ON)
        set(LOG4CPLUS_HAVE_GETTIMEOFDAY ON)
        set(LOG4CPLUS_HAVE_CLOCK_GETTIME OFF)
        set(LOG4CPLUS_HAVE_GETPID ON)
        set(LOG4CPLUS_HAVE_FTIME ON)
        set(LOG4CPLUS_HAVE_STAT ON)
        set(LOG4CPLUS_HAVE_LSTAT ON)
        set(LOG4CPLUS_HAVE_HTONS ON)
        set(LOG4CPLUS_HAVE_NTOHS ON)
        set(LOG4CPLUS_HAVE_HTONL ON)
        set(LOG4CPLUS_HAVE_NTOHL ON)

        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/log4cplus/config/defines.hxx.in
            ${CMAKE_CURRENT_SOURCE_DIR}/include/log4cplus/config/defines.hxx
            )
    else()
        INCLUDE(CheckIncludeFiles)

        CHECK_INCLUDE_FILES ("sys/types.h" LOG4CPLUS_HAVE_SYS_TYPES_H)
        CHECK_INCLUDE_FILES ("sys/socket.h" LOG4CPLUS_HAVE_SYS_SOCKET_H)
        CHECK_INCLUDE_FILES ("sys/time.h" LOG4CPLUS_HAVE_SYS_TIME_H)
        CHECK_INCLUDE_FILES ("sys/timeb.h" LOG4CPLUS_HAVE_SYS_TIMEB_H)
        CHECK_INCLUDE_FILES ("sys/stat.h" LOG4CPLUS_HAVE_SYS_STAT_H)
        CHECK_INCLUDE_FILES ("syslog.h" LOG4CPLUS_HAVE_SYSLOG_H)
        CHECK_INCLUDE_FILES ("netinet/in.h" LOG4CPLUS_HAVE_NETINET_IN_H)
        CHECK_INCLUDE_FILES ("netdb.h" LOG4CPLUS_HAVE_NETDB_H)
        CHECK_INCLUDE_FILES ("unistd.h" LOG4CPLUS_HAVE_UNISTD_H)
        CHECK_INCLUDE_FILES ("stdio.h" LOG4CPLUS_HAVE_STDIO_H)
        CHECK_INCLUDE_FILES ("wchar.h" LOG4CPLUS_HAVE_WCHAR_H)
        CHECK_INCLUDE_FILES ("stdarg.h" LOG4CPLUS_HAVE_STDARG_H)
        CHECK_INCLUDE_FILES ("time.h" LOG4CPLUS_HAVE_TIME_H)
        CHECK_INCLUDE_FILES ("errno.h" LOG4CPLUS_HAVE_ERRNO_H)

        #dnl Checks for library functions.

        INCLUDE (CheckFunctionExists)

        CHECK_FUNCTION_EXISTS(gmtime_r LOG4CPLUS_HAVE_GMTIME_R)
        CHECK_FUNCTION_EXISTS(localtime_r LOG4CPLUS_HAVE_LOCALTIME_R)
        CHECK_FUNCTION_EXISTS(gettimeofday LOG4CPLUS_HAVE_GETTIMEOFDAY)
        CHECK_FUNCTION_EXISTS(clock_gettime LOG4CPLUS_HAVE_CLOCK_GETTIME)
        CHECK_FUNCTION_EXISTS(getpid LOG4CPLUS_HAVE_GETPID)
        CHECK_FUNCTION_EXISTS(ftime LOG4CPLUS_HAVE_FTIME)
        CHECK_FUNCTION_EXISTS(stat LOG4CPLUS_HAVE_STAT)
        CHECK_FUNCTION_EXISTS(lstat LOG4CPLUS_HAVE_LSTAT)
        CHECK_FUNCTION_EXISTS(htons LOG4CPLUS_HAVE_HTONS)
        CHECK_FUNCTION_EXISTS(ntohs LOG4CPLUS_HAVE_NTOHS)
        CHECK_FUNCTION_EXISTS(htonl LOG4CPLUS_HAVE_HTONL)
        CHECK_FUNCTION_EXISTS(ntohl LOG4CPLUS_HAVE_NTOHL)

        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/log4cplus/config/defines.hxx.in
            ${CMAKE_CURRENT_SOURCE_DIR}/include/log4cplus/config/defines.hxx
            )
    endif()
endif()


if (NOT TARGET_ALREADY_EXISTS)

    if ("${UNIX}" OR "${CYGWIN}")
      set (log4cplus_sources ${log4cplus_sources}
        src/socket-unix.cxx)
      set (log4cplus_headers ${log4cplus_headers}
        include/log4cplus/config/defines.hxx
        include/log4cplus/helpers/syncprims-pthreads.h)
    elseif (WIN32)
      set (log4cplus_sources ${log4cplus_sources}
        src/socket-win32.cxx
        src/win32debugappender.cxx
        src/nteventlogappender.cxx
        src/win32consoleappender.cxx)
      set (log4cplus_headers ${log4cplus_headers}
        include/log4cplus/helpers/syncprims-win32.h)

      # Comment out the following lines if you want non-Unicode version.
      add_definitions (-DUNICODE -D_UNICODE -UMBCS -U_MBCS)
      set (log4cplus_postfix "${log4cplus_postfix}U")
      
      #add_definitions (-DLOG4CPLUS_STATIC)
      #set (log4cplus_postfix "${log4cplus_postfix}S")
    endif ()

    if (VERBOSE)
        message (STATUS "Headers: ${log4cplus_headers}")
        message (STATUS "Sources: ${log4cplus_sources}")
    endif()

    include_directories ("include")
    set (log4cplus_all_sources ${log4cplus_sources} ${log4cplus_headers})

    add_library (log4cplus STATIC ${log4cplus_all_sources})
    set_target_properties(log4cplus PROPERTIES FOLDER "Libraries")
    #add_library (log4cplus SHARED ${log4cplus_all_sources})
    target_link_libraries (log4cplus ${CMAKE_THREAD_LIBS_INIT})

    set_target_properties (log4cplus PROPERTIES
      VERSION "${log4cplus_version_major}.${log4cplus_version_minor}"
      SOVERSION "${log4cplus_soversion}"
      COMPILE_FLAGS "-DINSIDE_LOG4CPLUS")
    if (WIN32)
      set_target_properties (log4cplus PROPERTIES
        DEBUG_POSTFIX "${log4cplus_postfix}D"
        RELEASE_POSTFIX "${log4cplus_postfix}"
        MINSIZEREL_POSTFIX "${log4cplus_postfix}"
        RELWITHDEBINFO_POSTFIX "${log4cplus_postfix}")
      target_link_libraries (log4cplus ws2_32 advapi32)
    endif ()

endif()
#add_subdirectory (loggingserver)
#add_subdirectory (tests)

if (FIREBREATH)
    LIST(APPEND FBLIB_LIBRARIES log4cplus)
    LIST(APPEND FBLIB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
    export_project_dependencies()
endif()
