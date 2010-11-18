/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sep 30, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html
            
Copyright 2010 Facebook, Inc
\**********************************************************/

#pragma once
#ifndef H_FB_LOGGING
#define H_FB_LOGGING

#include <sstream>

#if !FB_NO_LOGGING_MACROS
#  if defined(__GNUC__)
#    define FBLOG_FUNCTION() __PRETTY_FUNCTION__
#  elif defined(_MSC_VER)
#    define FBLOG_FUNCTION() __FUNCTION__
#  else
#    define FBLOG_FUNCTION() __func__
#  endif
#  define FBLOG_LOG_BODY(type, src, msg) \
    do { \
        std::ostringstream os; \
        os << msg; \
        FB::Log:: type (src, os.str(), __FILE__, __LINE__, FBLOG_FUNCTION()); \
    } while(0)
#endif

#if !FB_NO_LOGGING_MACROS
#  define FBLOG_TRACE(src, msg) FBLOG_LOG_BODY(trace, src, msg)
#  define FBLOG_DEBUG(src, msg) FBLOG_LOG_BODY(debug, src, msg)
#  define FBLOG_INFO(src, msg) FBLOG_LOG_BODY(info, src, msg)
#  define FBLOG_WARN(src, msg) FBLOG_LOG_BODY(warn, src, msg)
#  define FBLOG_ERROR(src, msg) FBLOG_LOG_BODY(error, src, msg)
#  define FBLOG_FATAL(src, msg) FBLOG_LOG_BODY(fatal, src, msg)
#else 
#  define FBLOG_TRACE(src, msg) 
#  define FBLOG_DEBUG(src, msg)
#  define FBLOG_INFO(src, msg)
#  define FBLOG_WARN(src, msg)
#  define FBLOG_ERROR(src, msg)
#  define FBLOG_FATAL(src, msg)
#endif

namespace FB { namespace Log {

    void initLogging();
    void stopLogging();

    void trace(const std::string& src, const std::string& msg, const char *file, int line, const char *fn);
    void debug(const std::string& src, const std::string& msg, const char *file, int line, const char *fn);
    void  info(const std::string& src, const std::string& msg, const char *file, int line, const char *fn);
    void  warn(const std::string& src, const std::string& msg, const char *file, int line, const char *fn);
    void error(const std::string& src, const std::string& msg, const char *file, int line, const char *fn);
    void fatal(const std::string& src, const std::string& msg, const char *file, int line, const char *fn);

    std::wstring getLogFilename();

}; };

#endif
