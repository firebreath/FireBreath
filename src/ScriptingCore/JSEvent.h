/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Dec 17, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html
            
Copyright 2010 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_JSEVENT
#define H_FB_JSEVENT

#include "APITypes.h"
#include "JSAPIAuto.h"

namespace FB {

    FB::VariantMap CreateEvent(const FB::JSAPIPtr& api, const std::string& name, const FB::VariantMap &members, const FB::VariantList &arguments);
    FB::VariantMap CreateEvent(const FB::JSAPIPtr& api, const std::string& name, const FB::VariantMap &members);
    FB::VariantMap CreateEvent(const FB::JSAPIPtr& api, const std::string& name, const FB::VariantList &arguments);

};

#endif

