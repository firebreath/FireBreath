/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 24, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_FB_JSAPI
#define H_FB_JSAPI

#include "APITypes.h"
#include "AutoPtr.h"
#include <stdexcept>

namespace FB
{
    class EventHandlerObject;
    class BrowserHostWrapper;
    struct script_error : std::exception
    {
        script_error(std::string error)
            : m_error(error)
        { }
        virtual const char* what() { 
            return m_error.c_str(); 
        }
        std::string m_error;
    };

    struct invalid_arguments : script_error
    {
        invalid_arguments()
            : script_error("Invalid Arguments")
        { }
    };

    struct object_invalidated : script_error
    {
        object_invalidated()
            : script_error("This object is no longer valid")
        { }
    };
    
    struct invalid_member : script_error
    {
        invalid_member(std::string memberName)
            : script_error("The specified member does not exist: " + memberName)
        { }
    };

    class JSAPI
    {
    public:
        JSAPI(void);
        virtual ~JSAPI(void);

    // Support Reference counting
    protected:
        unsigned int m_refCount;

    public:
        void AddRef();
        unsigned int Release();

        void invalidate();

    protected:
        // Used to fire an event to the listeners attached to this JSAPI
        virtual void FireEvent(std::string eventName, std::vector<variant>&);

    public:
        // Methods for managing event sinks (BrowserHostWrapper objects)
        virtual void attachEventSink(BrowserHostWrapper *sink);
        virtual void detachEventSink(BrowserHostWrapper *sink);

        // Methods for registering properties and functions to the auto-table
        virtual void registerMethod(std::string name, CallMethodPtr func);
        virtual void registerProperty(std::string name, GetPropPtr getFunc, SetPropPtr setFunc);

        virtual void registerEventMethod(std::string name, EventHandlerObject *event);
        virtual void unregisterEventMethod(std::string name, EventHandlerObject *event);
        virtual EventHandlerObject *getDefaultEventMethod(std::string name);
        virtual void setDefaultEventMethod(std::string name, EventHandlerObject *event);

        // Methods to query existance of members on the API
        virtual bool HasMethod(std::string methodName);
        virtual bool HasProperty(std::string propertyName);
        virtual bool HasProperty(int idx);
        virtual bool HasEvent(std::string eventName);

        // Methods to manage properties on the API
        virtual variant GetProperty(std::string propertyName);
        virtual void SetProperty(std::string propertyName, const variant value);
        virtual variant GetProperty(int idx);
        virtual void SetProperty(int idx, const variant value);

        // Methods to manage methods on the API
        virtual variant Invoke(std::string methodName, std::vector<variant>& args);

    public:
        // Example function call and read-only property; override these if desired in derived classes
        virtual variant callToString(std::vector<variant>&args);
        virtual variant getValid();

    protected:
        MethodMap m_methodMap;
        PropertyMap m_propertyMap;
        EventMultiMap m_eventMap;
        EventSingleMap m_defEventMap;
        EventSinkMap m_sinkMap;
        
        bool m_valid;
    };

};
#endif