/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sep 30, 2010
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html
            
Copyright 2010 Facebook, Inc
\**********************************************************/

#pragma once
#ifndef H_FB_LOGGING
#define H_FB_LOGGING

#define FBLOG_TRACE(src, msg) FB::Log::trace(src, msg, __FILE__, __LINE__)
#define FBLOG_DEBUG(src, msg) FB::Log::debug(src, msg, __FILE__, __LINE__)
#define FBLOG_INFO(src, msg) FB::Log::info(src, msg, __FILE__, __LINE__)
#define FBLOG_WARN(src, msg) FB::Log::warn(src, msg, __FILE__, __LINE__)
#define FBLOG_ERROR(src, msg) FB::Log::error(src, msg, __FILE__, __LINE__)
#define FBLOG_FATAL(src, msg) FB::Log::fatal(src, msg, __FILE__, __LINE__)

namespace FB { namespace Log {

    void initLogging();
    void stopLogging();

    void trace(const char *, const char *, const char *file, int line);
    void debug(const char *, const char *, const char *file, int line);
    void info(const char *, const char *, const char *file, int line);
    void warn(const char *, const char *, const char *file, int line);
    void error(const char *, const char *, const char *file, int line);
    void fatal(const char *, const char *, const char *file, int line);

    std::wstring getLogFilename();

}; };

#endif
