/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Nov 23, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html
            
Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_JSAPISecureProxy
#define H_FB_JSAPISecureProxy

#include "APITypes.h"
#include <boost/make_shared.hpp>
#include "JSAPISecure.h"

namespace FB
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  JSAPISecureProxy
    ///
    /// @brief  JSAPISecure Wrapper -- this can wrap a JSAPISecure object to be used in a different
    ///         security zone (to provide higher or lower access privileges to this object than is
    ///         accessible using the zone that the JSAPISecure object was created with)
    /// 
    /// There are cases when you may want to pass a JSAPI object into another browser / browserhost
    /// instance (even just such as another page).  This class makes it possible to return a JSAPI object
    /// from a different instance of the plugin into a page, or presumably even a JSObject from one
    /// page into another (be *really* careful with that).  Note that not all JSAPI objects are linked
    /// to a BrowserHost instance, and thus might not need this wrapper class to function correctly
    /// 
    /// @see FB::JSAPI
    /// @see FB::JSAPIAuto
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class JSAPISecureProxy : public JSAPI
    {
    protected:
        JSAPISecureProxy(const FB::SecurityZone zone, const FB::JSAPIPtr &inner);
    public:
        static boost::shared_ptr<JSAPISecureProxy> create(const FB::SecurityZone zone, const FB::JSAPIPtr &inner);
        virtual ~JSAPISecureProxy(void);

        FB::JSAPISecurePtr getInnerAPI() { return m_api; }
    protected:
        FB::SecurityZone m_zone;
        FB::JSAPISecurePtr m_api;

    public:

        void invalidate();
        virtual void getMemberNames(std::vector<std::string> &nameVector);

        virtual size_t getMemberCount();
        virtual bool HasMethod(const std::string& methodName);
        virtual bool HasProperty(const std::string& propertyName);
        virtual bool HasProperty(int idx);

        virtual bool HasEvent(const std::string& eventName);
        virtual bool HasEvent(const std::wstring& eventName);

        virtual variant GetProperty(const std::string& propertyName);

        virtual void SetProperty(const std::string& propertyName, const variant& value);

        virtual variant GetProperty(int idx);
        virtual void SetProperty(int idx, const variant& value);
        virtual variant Invoke(const std::string& methodName, const std::vector<variant>& args);
    };

};
#endif
