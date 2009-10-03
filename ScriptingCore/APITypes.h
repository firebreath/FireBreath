/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 24, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_FB_METHODINFO
#define H_FB_METHODINFO

#include <string>
#include <vector>
#include <map>
#include "any.h"

namespace FB
{
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
    typedef void (JSAPI::*SetPropPtr)(const variant &value);
    struct PropertyInfo {
        PropertyInfo() : getFunc(NULL), setFunc(NULL) { }
        PropertyInfo(GetPropPtr getFunc, SetPropPtr setFunc) : getFunc(getFunc), setFunc(setFunc) { }
        PropertyInfo(const PropertyInfo &rh) : getFunc(rh.getFunc), setFunc(rh.setFunc) { }
        GetPropPtr getFunc;
        SetPropPtr setFunc;
    };

    typedef std::map<std::string, PropertyInfo> PropertyMap;

    struct EventInfo {
        EventInfo() : context(NULL) { }
        EventInfo(variant func, void *context) : func(func), context(context) { }
        EventInfo(const EventInfo &rh) : func(rh.func), context(rh.context) { }
        variant func;
        void *context;
    };
    typedef std::pair<std::string, EventInfo> EventPair;
    typedef std::multimap<std::string, EventInfo> EventMap;

    class BrowserHostWrapper;
    typedef std::map<void *, BrowserHostWrapper *> EventSinkMap;
}

#endif