#ifndef H_FB_JSAPI
#define H_FB_JSAPI

#include "APITypes.h"

namespace FB
{
    class JSAPI
    {
    public:
        JSAPI(void);
        virtual ~JSAPI(void);

    public:
        // Methods for registering properties and functions to the auto-table
        virtual void registerMethod(std::string name, CallMethodPtr func);
        virtual void registerProperty(std::string name, GetPropPtr getFunc, SetPropPtr setFunc);
        virtual void registerEvent(std::string name);

        // Methods to query existance of members on the API
        virtual bool HasMethod(std::string methodName);
        virtual bool HasProperty(std::string propertyName);
        virtual bool HasPropertyIndex(int idx);
        virtual bool HasEvent(std::string eventName);

        // Methods to manage properties on the API
        virtual bool GetProperty(std::string propertyName, variant &retVal);
        virtual bool SetProperty(std::string propertyName, variant &value);
        virtual bool GetProperty(int idx, variant &retVal);
        virtual bool SetProperty(int idx, variant &value);

        // Methods to manage methods on the API
        virtual bool Invoke(std::string methodName, variant *args, int argCount, variant &retVal);

    public:
        // Example function call and read-only property; override these if desired in derived classes
        virtual bool callToString(variant *args, int argCount, variant &retVal);
        virtual bool getValid(variant &retVal);

    protected:
        MethodMap m_methodMap;
        PropertyMap m_propertyMap;
        EventMap m_eventMap;
    };

}
#endif