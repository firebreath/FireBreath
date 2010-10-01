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

void FB::Log::trace(const char *, const char *, const char *file, int line) {}
void FB::Log::debug(const char *, const char *, const char *file, int line) {}
void FB::Log::info(const char *, const char *, const char *file, int line) {}
void FB::Log::warn(const char *, const char *, const char *file, int line) {}
void FB::Log::error(const char *, const char *, const char *file, int line) {}
void FB::Log::fatal(const char *, const char *, const char *file, int line) {}

std::wstring getLogFilename();
