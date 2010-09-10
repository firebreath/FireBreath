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

#ifndef H_FB_JSAPI
#define H_FB_JSAPI

#include "APITypes.h"
#include "AutoPtr.h"
#include <stdexcept>
#include <boost/enable_shared_from_this.hpp>

namespace FB
{
    class BrowserObjectAPI;
    class BrowserHostWrapper;
    struct script_error : std::exception
    {
        script_error(const std::string& error)
            : m_error(error)
        { }
        ~script_error() throw() { }
        virtual const char* what() const throw() { 
            return m_error.c_str(); 
        }
        std::string m_error;
    };

    struct invalid_arguments : script_error
    {
        invalid_arguments()
            : script_error("Invalid Arguments")
        { }
        ~invalid_arguments() throw() { }

        invalid_arguments(const std::string& error)
            : script_error(error)
        { }
    };

    struct object_invalidated : script_error
    {
        object_invalidated()
            : script_error("This object is no longer valid")
        { }
        ~object_invalidated() throw() { }
    };
    
    struct invalid_member : script_error
    {
        invalid_member(const std::string& memberName)
            : script_error("The specified member does not exist: " + memberName)
        { }
        ~invalid_member() throw() { }
    };

    class JSAPI : public boost::enable_shared_from_this<JSAPI>
    {
    public:
        JSAPI(void);
        virtual ~JSAPI(void);

    public:
        JSAPIPtr shared_ptr()
        {
            return shared_from_this();
        }

        void invalidate();

    protected:
        // Used to fire an event to the listeners attached to this JSAPI
        virtual void FireEvent(const std::wstring& eventName, const std::vector<variant>&);
        virtual void FireEvent(const std::string& eventName, const std::vector<variant>&);

    public:
        virtual void registerEvent(const std::string& name);
        virtual void registerEvent(const std::wstring& name);
        virtual void registerEventMethod(const std::string& name, JSObject& event);
        virtual void registerEventMethod(const std::wstring& name, JSObject& event);
        virtual void unregisterEventMethod(const std::string& name, JSObject& event);
        virtual void unregisterEventMethod(const std::wstring& name, JSObject& event);
        virtual void registerEventInterface(JSObject& event);
        virtual void unregisterEventInterface(JSObject& event);
        virtual JSObject getDefaultEventMethod(const std::wstring& name);
        virtual JSObject getDefaultEventMethod(const std::string& name);
        virtual void setDefaultEventMethod(const std::wstring& name, JSObject& event);
        virtual void setDefaultEventMethod(const std::string& name, JSObject& event);

        // Methods for enumeration
        virtual void getMemberNames(std::vector<std::wstring> &nameVector);
        virtual void getMemberNames(std::vector<std::string> &nameVector) = 0;
        virtual size_t getMemberCount() = 0;

        // Methods to query existance of members on the API
        virtual bool HasMethod(const std::wstring& methodName);
        virtual bool HasMethod(const std::string& methodName) = 0;
        virtual bool HasProperty(const std::wstring& propertyName);
        virtual bool HasProperty(const std::string& propertyName) = 0;
        virtual bool HasProperty(int idx) = 0;
        virtual bool HasEvent(const std::string& eventName);
        virtual bool HasEvent(const std::wstring& eventName);

        // Methods to manage properties on the API
        virtual variant GetProperty(const std::wstring& propertyName);
        virtual variant GetProperty(const std::string& propertyName) = 0;
        virtual void SetProperty(const std::wstring& propertyName, const variant& value);
        virtual void SetProperty(const std::string& propertyName, const variant& value) = 0;
        virtual variant GetProperty(int idx) = 0;
        virtual void SetProperty(int idx, const variant& value) = 0;

        // Methods to manage methods on the API
        virtual variant Invoke(const std::wstring& methodName, const std::vector<variant>& args);
        virtual variant Invoke(const std::string& methodName, const std::vector<variant>& args) = 0;

    protected:
        EventMultiMap m_eventMap;
        EventSingleMap m_defEventMap;
        EventIFaceMap m_evtIfaces;
                
        bool m_valid;
    };

};
#endif
