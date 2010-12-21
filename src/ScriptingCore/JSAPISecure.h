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

#include "APITypes.h"
#include <list>
#include "JSAPIAuto.h"

namespace FB {
    class JSAPISecureProxy;
    typedef boost::weak_ptr<JSAPISecureProxy> JSAPISecureProxyWeakPtr;

    class JSAPISecure : public FB::JSAPIAuto
    {
    public:
        JSAPISecure(const SecurityZone securityLevel, const std::string& description = "<JSAPI-Secure driven Javascript Object>");
        virtual ~JSAPISecure();

		virtual void setSecurityZone(const SecurityZone securityLevel);

        virtual void registerMethod(const std::string& name, const SecurityZone securityLevel, const CallMethodFunctor& func);
        virtual void registerProperty(const std::string& name, const SecurityZone securityLevel, const PropertyFunctors& propFuncs);
        virtual void getMemberNames(const SecurityZone securityLevel, std::vector<std::string> &nameVector);
        virtual void getMemberNames(std::vector<std::string> &nameVector);
        virtual size_t getMemberCount(const SecurityZone securityLevel);
        virtual size_t getMemberCount();

        virtual bool HasMethod(const std::string& methodName);
        virtual bool HasMethod(const FB::SecurityZone &zone, const std::string& methodName);
        virtual bool HasProperty(const std::string& propertyName);
        virtual bool HasProperty(const FB::SecurityZone &zone, const std::string& propertyName);
        virtual bool HasProperty(int idx);
        virtual bool HasProperty(const FB::SecurityZone &zone, int idx);

        virtual variant GetProperty(const std::string& propertyName);
        virtual variant GetProperty(const FB::SecurityZone &zone, const std::string& propertyName);
        virtual void SetProperty(const std::string& propertyName, const variant& value);
        virtual void SetProperty(const FB::SecurityZone &zone, const std::string& propertyName, const variant& value);
        virtual variant GetProperty(int idx);
        virtual variant GetProperty(const FB::SecurityZone &zone, int idx);
        virtual void SetProperty(int idx, const variant& value);
        virtual void SetProperty(const FB::SecurityZone &zone, int idx, const variant& value);
        virtual variant Invoke(const std::string& methodName, const std::vector<variant>& args);
        virtual variant Invoke(const FB::SecurityZone &zone, const std::string& methodName, const std::vector<variant>& args);

        virtual bool propertyAccessible( SecurityZone m_zone, const std::string& propertyName );
        virtual bool methodAccessible( SecurityZone m_zone, const std::string& methodName );
    protected:
        SecurityZone m_zone;
    };
    typedef boost::shared_ptr<FB::JSAPISecure> JSAPISecurePtr;
    typedef boost::weak_ptr<FB::JSAPISecure> JSAPISecureWeakPtr;
};

#endif // FB_JSAPIAUTO_H