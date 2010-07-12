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

namespace FB
{
    class BrowserObjectAPI;
    class BrowserHostWrapper;
    struct script_error : std::exception
    {
        script_error(std::string error)
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
        invalid_member(std::string memberName)
            : script_error("The specified member does not exist: " + memberName)
        { }
        ~invalid_member() throw() { }
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
        virtual void registerEvent(const std::string& name);
        virtual void registerEventMethod(std::string name, BrowserObjectAPI *event);
        virtual void unregisterEventMethod(std::string name, BrowserObjectAPI *event);
        virtual void registerEventInterface(BrowserObjectAPI *event);
        virtual void unregisterEventInterface(BrowserObjectAPI *event);
        virtual BrowserObjectAPI *getDefaultEventMethod(std::string name);
        virtual void setDefaultEventMethod(std::string name, BrowserObjectAPI *event);

        // Methods for enumeration
        virtual void getMemberNames(std::vector<std::string> &nameVector) = 0;
        virtual size_t getMemberCount() = 0;

        // Methods to query existance of members on the API
        virtual bool HasMethod(std::string methodName) = 0;
        virtual bool HasProperty(std::string propertyName) = 0;
        virtual bool HasProperty(int idx) = 0;
        virtual bool HasEvent(std::string eventName);

        // Methods to manage properties on the API
        virtual variant GetProperty(std::string propertyName) = 0;
        virtual void SetProperty(std::string propertyName, const variant value) = 0;
        virtual variant GetProperty(int idx) = 0;
        virtual void SetProperty(int idx, const variant value) = 0;

        // Methods to manage methods on the API
        virtual variant Invoke(std::string methodName, std::vector<variant>& args) = 0;

    protected:
        EventMultiMap m_eventMap;
        EventSingleMap m_defEventMap;
        EventIFaceMap m_evtIfaces;
                
        bool m_valid;
    };

};
#endif