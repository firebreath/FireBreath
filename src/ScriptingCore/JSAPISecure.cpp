/**********************************************************\ 
Original Author: Richard Bateman

Created:    Oct 20, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#include "JSAPISecure.h"

FB::JSAPISecure::JSAPISecure(const FB::SecurityZone securityZone, const std::string& description) :
    m_zone(securityZone), FB::JSAPIAuto(description)
{
}

FB::JSAPISecure::~JSAPISecure()
{
}

void FB::JSAPISecure::registerMethod(const std::string& name, const FB::SecurityZone minZone, const FB::CallMethodFunctor& func)
{
    if (m_zone >= minZone)
        FB::JSAPIAuto::registerMethod(name, func);
}

void FB::JSAPISecure::registerProperty(const std::wstring& name, const FB::SecurityZone minZone, const FB::PropertyFunctors& propFuncs)
{
    if (m_zone >= minZone)
        FB::JSAPIAuto::registerProperty(name, propFuncs);
}