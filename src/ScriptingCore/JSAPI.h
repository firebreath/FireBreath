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

#pragma once
#ifndef H_FB_JSAPI
#define H_FB_JSAPI

#include "APITypes.h"
#include <stdexcept>
#include <boost/enable_shared_from_this.hpp>

namespace FB
{
    class JSObject;
    class BrowserHost;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @exception script_error
    ///
    /// @brief  Exception type; when thrown in a JSAPI method, a javascript exception will be thrown. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @exception invalid_arguments
    ///
    /// @brief  Thrown by a JSAPI object when the argument(s) provided to a SetProperty or Invoke
    ///         call are found to be invalid.  JSAPIAuto will throw this automatically if the argument
    ///         cannot be convert_cast to the type expected by the function.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @exception object_invalidated
    ///
    /// @brief  Thrown by a JSAPI object when a call is made on it after the object has been
    ///         invalidated.
    ///         
    /// This is particularly useful when you want to invalidate the object
    /// when the plugin gets released, as the PluginCore-derived Plugin object will usually get
    /// released before the JSAPI object
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    struct object_invalidated : script_error
    {
        object_invalidated()
            : script_error("This object is no longer valid")
        { }
        ~object_invalidated() throw() { }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @exception invalid_member
    ///
    /// @brief  Thrown when an Invoke, SetProperty, or GetProperty call is made for a member that is
    ///         invalid (does not exist, not accessible, only supports Get or Set, etc) 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    struct invalid_member : script_error
    {
        invalid_member(const std::string& memberName)
            : script_error("The specified member does not exist: " + memberName)
        { }
        ~invalid_member() throw() { }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  JSAPI
    ///
    /// @brief  JavaScript API class -- provides a javascript interface that can be exposed to the
    ///         browser.
    /// 
    /// JSAPI is the core class for all interaction with javascript.  All PluginCore-derived Plugin
    /// objects must provide a JSAPI object to provide the javascript interface for their &lt;object&gt;
    /// tag, and methods or properties of that object can return other JSAPI objects.
    /// 
    /// Important things to know about JSAPI objects:
    ///   - Unless you have unusual needs, you will most likely want to extend FB::JSAPIAuto instead
    ///     of extending JSAPI directly.
    ///   - Any time you work with a JSAPI object you should use it with a boost::shared_ptr. 
    ///     FB::JSAPIPtr is a typedef for a boost::shared_ptr<JSAPI> which may be useful.
    ///     -  From inside the object you can use the shared_ptr() method to get a shared_ptr for
    ///        "this"
    ///   - Objects passed in from javascript, including functions, will be passed in as FB::JSObject
    ///     objects which extend JSAPI.
    ///
    /// @author Richard Bateman
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class JSAPI : public boost::enable_shared_from_this<JSAPI>
    {
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn JSAPI(void)
        ///
        /// @brief  Default constructor. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        JSAPI(void);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual ~JSAPI(void)
        ///
        /// @brief  Finaliser. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~JSAPI(void);

    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn JSAPIPtr shared_ptr()
        ///
        /// @brief  Gets the shared pointer for "this"
        ///
        /// @return JSAPIPtr for "this"
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        JSAPIPtr shared_ptr()
        {
            return shared_from_this();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn void invalidate()
        ///
        /// @brief  Invalidates this object.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void invalidate();

    protected:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual void FireEvent(const std::wstring& eventName, const std::vector<variant> &args)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void FireEvent(const std::wstring& eventName, const std::vector<variant> &args);
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void FireEvent(const std::string& eventName, const std::vector<variant> &args)
        ///
        /// @brief  Fires an event into javascript asynchronously
        /// 		
        /// This fires an event to all handlers attached to the given event in javascript.
        /// 
        /// IE:
        /// @code
        ///      document.getElementByID("plugin").attachEvent("onload", function() { alert("loaded!"); });
        /// @endcode
        /// Firefox/Safari/Chrome/Opera:
        /// @code
        /// 	 // Note that the convention used by these browsers is that "on" is implied
        /// 	 document.getElementByID("plugin").addEventListener("load", function() { alert("loaded!"); }, false);;/.
        /// @endcode
        ///
        /// You can then fire the event -- from any thread -- from the JSAPI object like so:
        /// @code
        /// 	 FireEvent("onload", FB::variant_list_of("param1")(2)(3.0));
        /// @endcode
        /// 		
        /// Also note that registerEvent must be called from the constructor to register the event.
        /// @code
        /// 	 registerEvent("onload");
        /// @endcode
        /// 
        /// @param  eventName   Name of the event.  This event must start with "on"
        /// @param  args        The arguments that should be sent to each attached event handler
        ///
        /// @see registerEvent
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void FireEvent(const std::string& eventName, const std::vector<variant> &args);

    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void registerEvent(const std::string& name)
        ///
        /// @brief  Register event so that event listeners can be added/attached from javascript
        ///
        /// @param  name    The name of the event to register.  This event must start with "on"
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void registerEvent(const std::string& name);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual void registerEvent(const std::wstring& name)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void registerEvent(const std::wstring& name);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void registerEventMethod(const std::string& name, JSObjectPtr& event)
        ///
        /// @brief  Called by the browser to register an event handler method
        ///
        /// @param  name            The name. 
        /// @param  event           The event handler method. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void registerEventMethod(const std::string& name, JSObjectPtr& event);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual void registerEventMethod(const std::wstring& name, JSObjectPtr& event)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void registerEventMethod(const std::wstring& name, JSObjectPtr& event);
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void unregisterEventMethod(const std::string& name, JSObjectPtr& event)
        ///
        /// @brief  Called by the browser to unregister an event handler method
        ///
        /// @param  name            The name. 
        /// @param  event           The event handler method to unregister. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void unregisterEventMethod(const std::string& name, JSObjectPtr& event);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual void unregisterEventMethod(const std::wstring& name, JSObjectPtr& event)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void unregisterEventMethod(const std::wstring& name, JSObjectPtr& event);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void registerEventInterface(JSObjectPtr& event)
        ///
        /// @brief  Called by the browser to register a JSObject interface that handles events.  This is
        /// 		primarily used by IE.  Objects provided to this method are called when events are fired
        /// 		by calling a method of the event name on the event interface
        ///
        /// @param  event   The JSAPI interface 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void registerEventInterface(JSObjectPtr& event);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void unregisterEventInterface(JSObjectPtr& event)
        ///
        /// @brief  Called by the browser to unregister a JSObject interface that handles events.  
        ///
        /// @param  event   The JSAPI interface
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void unregisterEventInterface(JSObjectPtr& event);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual JSObjectPtr getDefaultEventMethod(const std::wstring& name)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual JSObjectPtr getDefaultEventMethod(const std::wstring& name);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual JSObjectPtr getDefaultEventMethod(const std::string& name)
        ///
        /// @brief  Called by the browser to get the default event handler method for an event.
        /// 		
        /// This is called when the following occurs iff onload is a registered event:
        /// @code
        /// 	 var handler = document.getElementByID("plugin").onload;
        /// @endcode
        /// 		
        /// @param  name    The event name. 
        ///
        /// @return The default event method. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual JSObjectPtr getDefaultEventMethod(const std::string& name);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void setDefaultEventMethod(const std::string& name, JSObjectPtr event)
        ///
        /// @brief  Called by the browser to set the default event handler method for an event.
        ///
        /// This is called when the following occurs iff onload is a registered event:
        /// @code
        /// 	 document.getElementByID("plugin").onload = function() { alert("loaded"); };
        /// @endcode
        ///
        /// @param  name    The event name
        /// @param  event   The event handler method. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setDefaultEventMethod(const std::string& name, JSObjectPtr event);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual void setDefaultEventMethod(const std::wstring& name, JSObjectPtr event)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setDefaultEventMethod(const std::wstring& name, JSObjectPtr event);

        virtual void getMemberNames(std::vector<std::wstring> &nameVector);
        virtual void getMemberNames(std::vector<std::wstring> *nameVector);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void getMemberNames(std::vector<std::string> &nameVector) = 0
        ///
        /// @brief  Called by the browser to enumerate the members of this JSAPI object
        /// 		
        /// This must be implemented by anything extending JSAPI directly.  JSAPIAuto implements this
        /// for you.
        ///
        /// @param [out] nameVector  The name vector. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void getMemberNames(std::vector<std::string> &nameVector) = 0;
        virtual void getMemberNames(std::vector<std::string> *nameVector);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual size_t getMemberCount() = 0
        ///
        /// @brief  Gets the member count. 
        ///
        /// @return The member count. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual size_t getMemberCount() = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual bool HasMethod(const std::wstring& methodName)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool HasMethod(const std::wstring& methodName);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool HasMethod(const std::string& methodName) = 0
        ///
        /// @brief  Query if the JSAPI object has the 'methodName' method. 
        ///
        /// @param  methodName  Name of the method. 
        ///
        /// @return true if method exists, false if not. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool HasMethod(const std::string& methodName) = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual bool HasProperty(const std::wstring& propertyName)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool HasProperty(const std::wstring& propertyName);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool HasProperty(const std::string& propertyName)
        ///
        /// @brief  Query if 'propertyName' is a valid property. 
        ///
        /// @param  propertyName    Name of the property. 
        ///
        /// @return true if property exists, false if not. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool HasProperty(const std::string& propertyName) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool HasProperty(int idx) = 0
        ///
        /// @brief  Query if the property at "idx" exists.
        /// 	
        /// This can be used for providing array-style access on your object.  For example, the following
        /// will result in a call to HasProperty with idx = 12:
        /// @code
        /// 	  document.getElementById("plugin")[12];
        /// @endcode 
        ///
        /// @param  idx Zero-based index of the property to check for
        ///
        /// @return true if property exists, false if not. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool HasProperty(int idx) = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool HasEvent(const std::string& eventName)
        ///
        /// @brief  Query if the event 'eventName' has been registered
        ///
        /// @param  eventName   Name of the event. 
        ///
        /// @return true if event registered, false if not. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool HasEvent(const std::string& eventName);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual bool HasEvent(const std::wstring& eventName)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool HasEvent(const std::wstring& eventName);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual variant GetProperty(const std::wstring& propertyName)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual variant GetProperty(const std::wstring& propertyName);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual variant GetProperty(const std::string& propertyName) = 0
        ///
        /// @brief  Gets a property value
        ///
        /// @param  propertyName    Name of the property. 
        ///
        /// @return The property value 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual variant GetProperty(const std::string& propertyName) = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual void SetProperty(const std::wstring& propertyName, const variant& value)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void SetProperty(const std::wstring& propertyName, const variant& value);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void SetProperty(const std::string& propertyName, const variant& value) = 0
        ///
        /// @brief  Sets the value of a property. 
        ///
        /// @param  propertyName    Name of the property. 
        /// @param  value           The value. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void SetProperty(const std::string& propertyName, const variant& value) = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual variant GetProperty(int idx) = 0
        ///
        /// @brief  Gets the value of an indexed property. 
        ///
        /// This can be used for providing array-style access on your object.  For example, the following
        /// will result in a call to GetProperty with idx = 12:
        /// @code
        /// 	  var i = document.getElementById("plugin")[12];
        /// @endcode 
        ///
        /// @param  idx Zero-based index of the property to get the value of. 
        ///
        /// @return The property value. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual variant GetProperty(int idx) = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void SetProperty(int idx, const variant& value) = 0
        ///
        /// @brief  Sets the value of an indexed property. 
        ///
        /// This can be used for providing array-style access on your object.  For example, the following
        /// will result in a call to SetProperty with idx = 12:
        /// @code
        /// 	  document.getElementById("plugin")[12] = "property value";
        /// @endcode 
        ///
        /// @param  idx     Zero-based index of the property to set the value of. 
        /// @param  value   The new property value. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void SetProperty(int idx, const variant& value) = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual variant Invoke(const std::wstring& methodName, const std::vector<variant>& args)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual variant Invoke(const std::wstring& methodName, const std::vector<variant>& args);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual variant Invoke(const std::string& methodName,
        /// const std::vector<variant>& args) = 0
        ///
        /// @brief  Called by the browser to invoke a method on the JSAPI object.
        ///
        /// @param  methodName  Name of the method. 
        /// @param  args        The arguments. 
        ///
        /// @return result of method call 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual variant Invoke(const std::string& methodName, const std::vector<variant>& args) = 0;

    protected:
        EventMultiMap m_eventMap;       // Stores event handlers
        EventSingleMap m_defEventMap;   // Stores event-as-property event handlers
        EventIFaceMap m_evtIfaces;      // Stores event interfaces
                
        bool m_valid;                   // Tracks if this object has been invalidated
    };

};
#endif
