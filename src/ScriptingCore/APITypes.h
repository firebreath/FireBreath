/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Sept 24, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_FB_APITYPES
#define H_FB_APITYPES

#include <string>
#include <vector>
#include <map>
#include <set>
#include <boost/function.hpp>
#include "AutoPtr.h"

namespace FB
{
    class BrowserObjectAPI;
    class BrowserHostWrapper;
    class JSAPI;
    class BrowserObjectAPI;
    class variant;

    // Variant list

    typedef std::vector<variant> VariantList;
    typedef std::map<std::string, variant> VariantMap;
    typedef std::set<std::string> StringSet;

    typedef FB::AutoPtr<FB::JSAPI> JSOutObject;
    typedef FB::AutoPtr<FB::BrowserObjectAPI> JSObject;
    typedef FB::AutoPtr<FB::BrowserHostWrapper> BrowserHost;

    struct CatchAll {
        typedef FB::VariantList value_type;
        FB::VariantList value;
    };

    // JSAPI methods

    class JSAPI;
    typedef variant (JSAPI::*CallMethodPtr)(std::vector<variant>&);
    struct MethodInfo {
        MethodInfo() : callFunc(NULL) { }
        MethodInfo(CallMethodPtr callFunc) : callFunc(callFunc) { }
        MethodInfo(const MethodInfo &rh) : callFunc(rh.callFunc) { }
        CallMethodPtr callFunc;
    };


    typedef std::map<std::string, MethodInfo> MethodMap;

    // JSAPI properties

    typedef variant (JSAPI::*GetPropPtr)();
    typedef void (JSAPI::*SetPropPtr)(const variant value);
    struct PropertyInfo {
        PropertyInfo() : getFunc(NULL), setFunc(NULL) { }
        PropertyInfo(GetPropPtr getFunc, SetPropPtr setFunc) : getFunc(getFunc), setFunc(setFunc) { }
        PropertyInfo(const PropertyInfo &rh) : getFunc(rh.getFunc), setFunc(rh.setFunc) { }
        GetPropPtr getFunc;
        SetPropPtr setFunc;
    };

    typedef std::map<std::string, PropertyInfo> PropertyMap;

    // new style JSAPI methods

    typedef boost::function<variant (std::vector<variant>&)> CallMethodFunctor;
    typedef std::map<std::string, CallMethodFunctor> MethodFunctorMap;

    // new style JSAPI properties

    typedef boost::function<FB::variant ()> GetPropFunctor;
    typedef boost::function<void (const FB::variant&)> SetPropFunctor;
    struct PropertyFunctors
    {
        GetPropFunctor get;
        SetPropFunctor set;
        PropertyFunctors() : get(), set() {}
        PropertyFunctors(const GetPropFunctor& get, const SetPropFunctor& set)
          : get(get), set(set) {}
        PropertyFunctors(const PropertyFunctors& p)
          : get(p.get), set(p.set) {}
        PropertyFunctors& operator=(const PropertyFunctors& rhs) {
            get = rhs.get;
            set = rhs.set;
            return *this;
        }
    };
    typedef std::map<std::string, PropertyFunctors> PropertyFunctorsMap;

    // JSAPI event handlers

    typedef std::pair<std::string, FB::JSObject> EventPair;
    typedef std::multimap<std::string, FB::JSObject> EventMultiMap;
    typedef std::map<void*, FB::JSObject> EventIFaceMap;
    typedef std::map<std::string, FB::JSObject> EventSingleMap;
}

// This needs to be included after all our classes are defined because it relies on types defined in this file
// TODO: can this be done better?
#include "variant.h"

#endif
