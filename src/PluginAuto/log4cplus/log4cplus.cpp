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

#include "FactoryBase.h"
#include "logging.h"
#include "utf8_tools.h"

#include "precompiled_headers.h" // On windows, everything above this line in PCH
#include "log4cplus/config/defines.hxx"
#include "log4cplus/logger.h"
#include "log4cplus/loglevel.h"
#include "log4cplus/layout.h"
#include "log4cplus/ndc.h"
#include "log4cplus/fileappender.h"
#include "log4cplus/win32debugappender.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/nullappender.h"

namespace 
{
    bool logging_started = false;
}

static log4cplus::LogLevel translate_logLevel(FB::Log::LogLevel ll);

void FB::Log::initLogging()
{
    if (logging_started)
        return;

    bool addedAppender = false;

    log4cplus::Logger logger = log4cplus::Logger::getInstance(L"FireBreath");

    FB::Log::LogLevel ll = getFactoryInstance()->getLogLevel();
    logger.setLogLevel(translate_logLevel(ll));

    FB::Log::LogMethodList mlist;
    getFactoryInstance()->getLoggingMethods(mlist);

    for (FB::Log::LogMethodList::const_iterator it = mlist.begin(); it != mlist.end(); ++it) {
        switch( it->first ) {
        case FB::Log::LogMethod_Console: {
#ifdef FB_WIN
            log4cplus::SharedAppenderPtr debugAppender(new log4cplus::Win32DebugAppender());
            std::auto_ptr<log4cplus::Layout> layout2(new log4cplus::TTCCLayout());
            debugAppender->setLayout(layout2);
            logger.addAppender(debugAppender);
            addedAppender = true;
#else
            log4cplus::SharedAppenderPtr debugAppender(new log4cplus::ConsoleAppender());
            std::auto_ptr<log4cplus::Layout> layout2(new log4cplus::TTCCLayout());
            debugAppender->setLayout(layout2);
            logger.addAppender(debugAppender);
            addedAppender = true;
#endif
            } break;
        case FB::Log::LogMethod_File: {
            log4cplus::SharedAppenderPtr fileAppender(new log4cplus::FileAppender(FB::utf8_to_wstring(it->second)));
            std::auto_ptr<log4cplus::Layout> layout(new log4cplus::TTCCLayout());
            fileAppender->setLayout(layout);
            logger.addAppender(fileAppender);
            addedAppender = true;
          }
        }
    }

    // If there are no other appenders, add a NULL appender
    if (!addedAppender) {
        log4cplus::SharedAppenderPtr nullAppender(new log4cplus::NullAppender());
        std::auto_ptr<log4cplus::Layout> layout3(new log4cplus::TTCCLayout());
        nullAppender->setLayout(layout3);
        logger.addAppender(nullAppender);
    }
    
    logging_started = true;
}

void FB::Log::stopLogging()
{
    log4cplus::Logger logger = log4cplus::Logger::getInstance(L"FireBreath");
    logger.shutdown();
    logging_started = false;
}

static log4cplus::LogLevel translate_logLevel(FB::Log::LogLevel ll){
    
    switch(ll) {
        case(FB::Log::LogLevel_Trace) :
            return log4cplus::TRACE_LOG_LEVEL;
        case(FB::Log::LogLevel_Debug) :
            return log4cplus::DEBUG_LOG_LEVEL;
        case(FB::Log::LogLevel_Info) : 
            return log4cplus::INFO_LOG_LEVEL;
        case(FB::Log::LogLevel_Warn) :
            return log4cplus::WARN_LOG_LEVEL;
        case(FB::Log::LogLevel_Error) : 
            return log4cplus::ERROR_LOG_LEVEL;
        default:
            return log4cplus::WARN_LOG_LEVEL;
    }
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

