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
#include "logging.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

void FB::Log::initLogging() {};
void FB::Log::stopLogging() {};

void FB::Log::trace(std::string, std::string, const char *file, int line, const char*) {}
void FB::Log::debug(std::string, std::string, const char *file, int line, const char*) {}
void FB::Log::info(std::string, std::string, const char *file, int line, const char*) {}
void FB::Log::warn(std::string, std::string, const char *file, int line, const char*) {}
void FB::Log::error(std::string, std::string, const char *file, int line, const char*) {}
void FB::Log::fatal(std::string, std::string, const char *file, int line, const char*) {}

