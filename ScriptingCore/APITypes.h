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
#include "any.h"
#include "AutoPtr.h"

namespace FB
{
    class EventHandlerObject;
    class BrowserHostWrapper;
    // Variant datatype used
    typedef cdiggins::any variant;

    class JSAPI;
    typedef variant (JSAPI::*CallMethodPtr)(std::vector<variant>&);
    struct MethodInfo {
        MethodInfo() : callFunc(NULL) { }
        MethodInfo(CallMethodPtr callFunc) : callFunc(callFunc) { }
        MethodInfo(const MethodInfo &rh) : callFunc(rh.callFunc) { }
        CallMethodPtr callFunc;
    };


    typedef std::map<std::string, MethodInfo> MethodMap;

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

    typedef std::pair<std::string, FB::AutoPtr<EventHandlerObject>> EventPair;
    typedef std::multimap<std::string, FB::AutoPtr<EventHandlerObject>> EventMultiMap;
    typedef std::map<void*, FB::AutoPtr<EventHandlerObject>> EventIFaceMap;
    typedef std::map<std::string, FB::AutoPtr<EventHandlerObject>> EventSingleMap;
}

#endif