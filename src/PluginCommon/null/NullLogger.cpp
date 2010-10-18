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

#include <string>
#include "../logging.h"

void FB::Log::initLogging() {};
void FB::Log::stopLogging() {};

void FB::Log::trace(const std::string&, const std::string&, const char *file, int line, const char*) {}
void FB::Log::debug(const std::string&, const std::string&, const char *file, int line, const char*) {}
void FB::Log::info(const std::string&, const std::string&, const char *file, int line, const char*) {}
void FB::Log::warn(const std::string&, const std::string&, const char *file, int line, const char*) {}
void FB::Log::error(const std::string&, const std::string&, const char *file, int line, const char*) {}
void FB::Log::fatal(const std::string&, const std::string&, const char *file, int line, const char*) {}

std::wstring getLogFilename();
