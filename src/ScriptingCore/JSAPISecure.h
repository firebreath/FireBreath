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

#pragma once
#ifndef FB_JSAPISECURE_H
#define FB_JSAPISECURE_H

#include "JSAPIAuto.h"

namespace FB {
    typedef int SecurityZone;

    class JSAPISecure : public FB::JSAPIAuto
    {
    public:
        enum SecurityLevel {
            SecurityScope_Public = 0,
            SecurityScope_Protected = 2,
            SecurityScope_Private = 4,
            SecurityScope_Local = 6
        };
    public:
        JSAPISecure(const SecurityZone securityLevel, const std::string& description = "<JSAPI-Secure driven Javascript Object>");
        virtual ~JSAPISecure();

        virtual void registerMethod(const std::string& name, const SecurityZone securityLevel, const CallMethodFunctor& func);
        virtual void registerProperty(const std::wstring& name, const SecurityZone securityLevel, const PropertyFunctors& propFuncs);

    protected:
        SecurityZone m_zone;
    };
};

#endif // FB_JSAPIAUTO_H