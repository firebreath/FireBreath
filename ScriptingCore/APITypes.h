#ifndef H_FB_METHODINFO
#define H_FB_METHODINFO

#include <string>
#include <map>
#include "any.h"

namespace FB
{
    // Variant datatype used
    typedef cdiggins::any variant;

    typedef bool (*CallMethodPtr)(variant *args, int argCount, variant &retVal);
    struct MethodInfo {
        MethodInfo() : callFunc(NULL) { }
        MethodInfo(CallMethodPtr callFunc) : callFunc(callFunc) { }
        MethodInfo(const MethodInfo &rh) : callFunc(rh.callFunc) { }
        CallMethodPtr callFunc;
    };


    typedef std::map<std::string, MethodInfo> MethodMap;

    typedef bool (*GetPropPtr)(variant &retVal);
    typedef bool (*SetPropPtr)(variant &value);
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
}

#endif