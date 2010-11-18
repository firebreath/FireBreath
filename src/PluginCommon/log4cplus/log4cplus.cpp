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

#include <string>
#include <boost/scoped_ptr.hpp>

#include "log4cplus/config/defines.hxx"
#include "log4cplus/logger.h"
#include "log4cplus/loglevel.h"
#include "log4cplus/layout.h"
#include "log4cplus/ndc.h"
#include "log4cplus/fileappender.h"
#include "log4cplus/win32debugappender.h"

#include "../logging.h"
#include "utf8_tools.h"

namespace 
{
    bool logging_started = false;
}

void FB::Log::initLogging()
{
    if (logging_started)
        return;

    log4cplus::Logger logger = log4cplus::Logger::getInstance(L"FireBreath");
#ifdef LOG_FILE
    log4cplus::SharedAppenderPtr fileAppender(new log4cplus::FileAppender(getLogFilename()));
    std::auto_ptr<log4cplus::Layout> layout(new log4cplus::TTCCLayout());
    fileAppender->setLayout(layout);
    logger.addAppender(fileAppender);
#endif

#if defined(_DEBUG) && defined(_WIN32)
    log4cplus::SharedAppenderPtr debugAppender(new log4cplus::Win32DebugAppender());
    std::auto_ptr<log4cplus::Layout> layout2(new log4cplus::TTCCLayout());
    debugAppender->setLayout(layout2);
    logger.addAppender(debugAppender);
#endif
    
    logging_started = true;
}

void FB::Log::stopLogging()
{
    log4cplus::Logger logger = log4cplus::Logger::getInstance(L"FireBreath");
    logger.shutdown();
    logging_started = false;
}

void FB::Log::trace(const std::string& src, const std::string& msg, const char *file, int line, const char *fn)
{
    LOG4CPLUS_TRACE(log4cplus::Logger::getInstance(L"FireBreath"), 
                    file << ":" << line << " - " << fn << " - " << FB::utf8_to_wstring(msg));
}
void FB::Log::debug(const std::string& src, const std::string& msg, const char *file, int line, const char *fn)
{
    LOG4CPLUS_DEBUG(log4cplus::Logger::getInstance(L"FireBreath"), 
                    file << ":" << line << " - " << fn << " - " << FB::utf8_to_wstring(msg));
}
void FB::Log::info(const std::string& src, const std::string& msg, const char *file, int line, const char *fn)
{
    LOG4CPLUS_INFO(log4cplus::Logger::getInstance(L"FireBreath"), 
                   file << ":" << line << " - " << fn << " - " << FB::utf8_to_wstring(msg));
}
void FB::Log::warn(const std::string& src, const std::string& msg, const char *file, int line, const char *fn)
{
    LOG4CPLUS_WARN(log4cplus::Logger::getInstance(L"FireBreath"), 
                   file << ":" << line << " - " << fn << " - " << FB::utf8_to_wstring(msg));
}
void FB::Log::error(const std::string& src, const std::string& msg, const char *file, int line, const char *fn)
{
    LOG4CPLUS_ERROR(log4cplus::Logger::getInstance(L"FireBreath"), 
                    file << ":" << line << " - " << fn << " - " << FB::utf8_to_wstring(msg));
}
void FB::Log::fatal(const std::string& src, const std::string& msg, const char *file, int line, const char *fn)
{
    LOG4CPLUS_ERROR(log4cplus::Logger::getInstance(L"FireBreath"), 
                    file << ":" << line << " - " << fn << " - " << FB::utf8_to_wstring(msg));
}

std::wstring FB::Log::getLogFilename()
{
#if defined(XP_WIN)
    return L"C:\\firebreath.log";
#elif defined(XP_UNIX)
    return L"~/firebreath.log";
#endif
}
