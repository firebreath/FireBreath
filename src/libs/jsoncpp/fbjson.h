/**********************************************************\
Original Author: Richard Bateman
 
Created:    Jun 28, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2011 Facebook, Inc
\**********************************************************/
#include "APITypes.h"
#include "json/json.h"

namespace FB { 
    Json::Value variantToJsonValue(const FB::variant &val);
    FB::variant jsonToVariantValue(const std::string& json);
    FB::variant jsonValueToVariant( Json::Value root );
} 
