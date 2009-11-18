/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 24, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_FB_APITYPES
#define H_FB_APITYPES

#include <string>
#include <vector>
#include <map>
#include <boost/function.hpp>
#include "variant.h"
#include "AutoPtr.h"

namespace FB
{
    class EventHandlerObject;
    class BrowserHostWrapper;

    // Variant list

    typedef std::vector<variant> VariantList;

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

    typedef boost::function<variant ()> GetPropFunctor;
    typedef boost::function<void (const variant)> SetPropFunctor;
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

    typedef std::pair<std::string, FB::AutoPtr<EventHandlerObject>> EventPair;
    typedef std::multimap<std::string, FB::AutoPtr<EventHandlerObject>> EventMultiMap;
    typedef std::map<void*, FB::AutoPtr<EventHandlerObject>> EventIFaceMap;
    typedef std::map<std::string, FB::AutoPtr<EventHandlerObject>> EventSingleMap;
}

#endif