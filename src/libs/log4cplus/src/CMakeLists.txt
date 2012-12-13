set (log4cplus_sources
  appenderattachableimpl.cxx
  appender.cxx
  asyncappender.cxx
  clogger.cxx
  configurator.cxx
  consoleappender.cxx
  cygwin-win32.cxx
  env.cxx
  factory.cxx
  fileappender.cxx
  fileinfo.cxx
  filter.cxx
  global-init.cxx
  hierarchy.cxx
  hierarchylocker.cxx
  layout.cxx
  log4judpappender.cxx
  lockfile.cxx
  logger.cxx
  loggerimpl.cxx
  loggingevent.cxx
  loggingmacros.cxx
  loglevel.cxx
  loglog.cxx
  logloguser.cxx
  mdc.cxx
  ndc.cxx
  nullappender.cxx
  objectregistry.cxx
  patternlayout.cxx
  pointer.cxx
  property.cxx
  queue.cxx
  rootlogger.cxx
  sleep.cxx
  snprintf.cxx
  socketappender.cxx
  socketbuffer.cxx
  socket.cxx
  stringhelper.cxx
  stringhelper-clocale.cxx
  stringhelper-cxxlocale.cxx
  stringhelper-iconv.cxx
  syncprims.cxx
  syslogappender.cxx
  threads.cxx
  timehelper.cxx
  tls.cxx
  version.cxx)

#message (STATUS "Type: ${UNIX}|${CYGWIN}|${WIN32}")

if ("${UNIX}" OR "${CYGWIN}")
  set (log4cplus_sources ${log4cplus_sources}
    socket-unix.cxx)
elseif (WIN32)
  set (log4cplus_sources ${log4cplus_sources}
    nteventlogappender.cxx
    socket-win32.cxx
    win32consoleappender.cxx
    win32debugappender.cxx)

  #add_definitions (-DLOG4CPLUS_STATIC)
  #set (log4cplus_postfix "${log4cplus_postfix}S")
endif ()

add_definitions (-DUNICODE -D_UNICODE -UMBCS -U_MBCS)

message("Adding library ${log4cplus}")
add_library (${log4cplus} ${log4cplus_sources})

set (log4cplus_LIBS ${CMAKE_THREAD_LIBS_INIT})
if (LIBRT)
  list (APPEND log4cplus_LIBS ${LIBRT})
endif ()
if (LIBPOSIX4)
  list (APPEND log4cplus_LIBS ${LIBPOSIX4})
endif ()
if (LIBCPOSIX)
  list (APPEND log4cplus_LIBS ${LIBCPOSIX})
endif ()
if (LIBSOCKET)
  list (APPEND log4cplus_LIBS ${LIBSOCKET})
endif ()
if (LIBNSL)
  list (APPEND log4cplus_LIBS ${LIBNSL})
endif ()
if (LOG4CPLUS_WITH_ICONV AND LIBICONV)
  list (APPEND log4cplus_LIBS ${LIBICONV})
endif ()
target_link_libraries (${log4cplus} ${log4cplus_LIBS})

set_target_properties (${log4cplus} PROPERTIES
  VERSION "${log4cplus_version_major}.${log4cplus_version_minor}.${log4cplus_version_patch}"
  SOVERSION "${log4cplus_soversion}"
  COMPILE_FLAGS "-DINSIDE_LOG4CPLUS")

if (WIN32)
  set_target_properties (${log4cplus} PROPERTIES
    DEBUG_POSTFIX "D")
  target_link_libraries (${log4cplus} ws2_32 advapi32)
endif ()

