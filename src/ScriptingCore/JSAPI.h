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
#include <list>
#include <deque>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include "JSExceptions.h"
#include "boost/thread/recursive_mutex.hpp"
#include "boost/thread/mutex.hpp"

namespace FB
{
    class JSObject;
    class BrowserHost;

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
    ///     -  From inside the object you can use the shared_from_this() method to get a shared_ptr for
    ///        "this"
    ///   - Objects passed in from javascript, including functions, will be passed in as FB::JSObject
    ///     objects which extend JSAPI.
    ///
    /// @author Richard Bateman
    /// @see FB::JSAPIAuto
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class JSAPI : public boost::enable_shared_from_this<JSAPI>, boost::noncopyable
    {
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn JSAPI(void)
        ///
        /// @brief  Default constructor.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        JSAPI(void) {}

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual ~JSAPI(void)
        ///
        /// @brief  Finaliser.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~JSAPI(void) {}

    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn void invalidate()
        ///
        /// @brief  Invalidates this object.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void invalidate() = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn void shutdown()
        ///
        /// @brief  Called to notify the object that the plugin is shutting down
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void shutdown() {}

    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public  void FB::JSAPI::pushZone(const SecurityZone& securityLevel)
        ///
        /// @brief  Pushes a new security level and locks a mutex (for every Push there *must* be a Pop!)
        ///
        /// This should be used to temporarily set the security zone of the API object. Note that this
        /// also locks a mutex to ensure that access to members under a non-default security level is
        /// serialized. Do not *ever* leave an unmatched push (a push with no pop after it). For safety,
        /// use the helper FB::scoped_zonelock:
        /// @code
        ///      // In the constructor
        ///      // Register protected members
        ///      {
        ///          FB::scoped_zonelock _l(this, SecurityScope_Protected);
        ///          registerMethod("start", make_method(this, &MyPluginAPI::start));
        ///      } // Zone automatically popped off
        ///      // Register private members
        ///      {
        ///          FB::scoped_zonelock _l(this, SecurityScope_Protected);
        ///          registerMethod("getDirectoryListing", make_method(this, &MyPluginAPI::getDirectoryListing));
        ///      } // Zone automatically popped off
        /// @endcode
        ///
        /// @param  securityLevel   const SecurityZone &    Zone id to push on the stack
        /// @since 1.4a3
        /// @see FB::scoped_zonelock
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void pushZone(const SecurityZone& securityLevel) { }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public  void FB::JSAPI::popZone()
        ///
        /// @brief  Pops off a security level and unlocks the mutex (for every Push there *must* be a Pop!)
        ///
        /// Seriously, it's far better to use FB::scoped_zonelock instead of using popZone and pushZone
        ///
        /// @returns void
        /// @since 1.4a3
        /// @see FB::scoped_zonelock
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void popZone() { }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public void setDefaultZone(const SecurityZone& securityLevel)
        ///
        /// @brief  Sets the default zone (the zone the class operates on before a push)
        ///
        /// @returns void
        /// @since 1.4a3
        /// @see FB::scoped_zonelock
        /// @see pushZone
        /// @see popZone
        /// @see getDefaultZone
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setDefaultZone(const SecurityZone& securityLevel) { }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public virtual SecurityZone getDefaultZone() const
        ///
        /// @brief  Gets the default zone (the zone the class operates on before a push)
        ///
        /// @returns SecurityZone the default zone
        /// @since 1.4a3
        /// @see FB::scoped_zonelock
        /// @see pushZone
        /// @see popZone
        /// @see getDefaultZone
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual SecurityZone getDefaultZone() const { return SecurityScope_Public; }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public SecurityZone getZone() const
        ///
        /// @brief  Gets the currently active zone
        ///
        /// @returns SecurityZone the current zone
        /// @since 1.4a3
        /// @see FB::scoped_zonelock
        /// @see pushZone
        /// @see popZone
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual SecurityZone getZone() const { return SecurityScope_Public; }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void registerEventMethod(const std::string& name, JSObjectPtr& event)
        ///
        /// @brief  Called by the browser to register an event handler method
        ///
        /// @param  name            The name.
        /// @param  event           The event handler method.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void registerEventMethod(const std::string& name, JSObjectPtr& event)
        {
            // TODO: add support for this in IDispatchAPI and NPObjectAPI
            throw FB::script_error("Not implemented");
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual void registerEventMethod(const std::wstring& name, JSObjectPtr& event)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void registerEventMethod(const std::wstring& name, JSObjectPtr& event)
        {
            registerEventMethod(wstring_to_utf8(name), event);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void unregisterEventMethod(const std::string& name, JSObjectPtr& event)
        ///
        /// @brief  Called by the browser to unregister an event handler method
        ///
        /// @param  name            The name.
        /// @param  event           The event handler method to unregister.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void unregisterEventMethod(const std::string& name, JSObjectPtr& event)
        {
            // TODO: add support for this in IDispatchAPI and NPObjectAPI
            throw FB::script_error("Not implemented");
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual void unregisterEventMethod(const std::wstring& name, JSObjectPtr& event)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void unregisterEventMethod(const std::wstring& name, JSObjectPtr& event)
        {
            unregisterEventMethod(wstring_to_utf8(name), event);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void registerEventInterface(const JSObjectPtr& event)
        ///
        /// @brief  Called by the browser to register a JSObject interface that handles events.  This is
        ///         primarily used by IE.  Objects provided to this method are called when events are fired
        ///         by calling a method of the event name on the event interface
        ///
        /// @param  event   The JSAPI interface
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void registerEventInterface(const JSObjectPtr& event)
        {
            // TODO: add support for this in IDispatchAPI and NPObjectAPI
            throw FB::script_error("Not implemented");
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void unregisterEventInterface(const JSObjectPtr& event)
        ///
        /// @brief  Called by the browser to unregister a JSObject interface that handles events.
        ///
        /// @param  event   The JSAPI interface
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void unregisterEventInterface(const JSObjectPtr& event)
        {
            // TODO: add support for this in IDispatchAPI and NPObjectAPI
            throw FB::script_error("Not implemented");
        }

        virtual void getMemberNames(std::vector<std::wstring> &nameVector) const
        {
            nameVector.clear();
            std::vector<std::string> utf8Vector;
            getMemberNames(utf8Vector);
            for (std::vector<std::string>::const_iterator it = utf8Vector.begin();
                    it != utf8Vector.end(); ++it) {
                std::wstring wStrVal(utf8_to_wstring(*it));
                nameVector.push_back(wStrVal);
            }
        }

        virtual void getMemberNames(std::vector<std::wstring> *nameVector) const
        {
            getMemberNames(*nameVector);
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void getMemberNames(std::vector<std::string> &nameVector) const = 0
        ///
        /// @brief  Called by the browser to enumerate the members of this JSAPI object
        ///
        /// This must be implemented by anything extending JSAPI directly.  JSAPIAuto implements this
        /// for you.
        ///
        /// @param [out] nameVector  The name vector.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void getMemberNames(std::vector<std::string> &nameVector) const = 0;
        virtual void getMemberNames(std::vector<std::string> *nameVector) const
        {
            getMemberNames(*nameVector);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual size_t getMemberCount() const = 0
        ///
        /// @brief  Gets the member count.
        ///
        /// @return The member count.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual size_t getMemberCount() const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual bool HasMethod(const std::wstring& methodName) const
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool HasMethod(const std::wstring& methodName) const
        {
            return HasMethod(wstring_to_utf8(methodName));
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool HasMethod(const std::string& methodName) const = 0
        ///
        /// @brief  Query if the JSAPI object has the 'methodName' method.
        ///
        /// @param  methodName  Name of the method.
        ///
        /// @return true if method exists, false if not.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool HasMethod(const std::string& methodName) const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual bool HasMethodObject(const std::wstring& methodObjName) const
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool HasMethodObject(const std::wstring& methodObjName) const
        {
            return HasMethodObject(wstring_to_utf8(methodObjName));
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool HasMethodObject(const std::string& methodObjName) const
        ///
        /// @brief  Query if 'methodObjName' is a valid methodObj.
        ///
        /// @param  methodObjName    Name of the method to fetch an object for.
        ///
        /// If this feature is supported
        ///
        /// @return true if methodObj exists, false if not.
        /// @since 1.4
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool HasMethodObject(const std::string& methodObjName) const { return false; }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual bool HasProperty(const std::wstring& propertyName) const
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool HasProperty(const std::wstring& propertyName) const
        {
            return HasProperty(wstring_to_utf8(propertyName));
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool HasProperty(const std::string& propertyName) const
        ///
        /// @brief  Query if 'propertyName' is a valid property.
        ///
        /// @param  propertyName    Name of the property.
        ///
        /// @return true if property exists, false if not.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool HasProperty(const std::string& propertyName) const = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool HasProperty(int idx) const = 0
        ///
        /// @brief  Query if the property at "idx" exists.
        ///
        /// This can be used for providing array-style access on your object.  For example, the following
        /// will result in a call to HasProperty with idx = 12:
        /// @code
        ///       document.getElementById("plugin")[12];
        /// @endcode
        ///
        /// @param  idx Zero-based index of the property to check for
        ///
        /// @return true if property exists, false if not.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool HasProperty(int idx) const = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual JSAPIPtr GetMethodObject(const std::wstring& methodObjName)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual JSAPIPtr GetMethodObject(const std::wstring& methodObjName)
        {
            return GetMethodObject(FB::wstring_to_utf8(methodObjName));
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual variant GetMethodObject(const std::string& methodObjName) = 0
        ///
        /// @brief  Gets a method object (JSAPI object that has a default method)
        ///
        /// Often it is preferable with the plugins to have the API return a JSAPI object as a
        /// property and then call the default method on that object.  This looks the same in
        /// javascript, except that you can save the function object if you want to.  See
        /// FB::JSFunction for an example of how to make a function object
        ///
        /// @param  methodObjName    Name of the methodObj.
        /// @return The methodObj value
        /// @since 1.4
        /// @see FB::JSFunction
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual JSAPIPtr GetMethodObject(const std::string& methodObjName) { return FB::JSAPIPtr(); }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual variant GetProperty(const std::wstring& propertyName)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual variant GetProperty(const std::wstring& propertyName)
        {
            return GetProperty(wstring_to_utf8(propertyName));
        }

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
        virtual void SetProperty(const std::wstring& propertyName, const variant& value)
        {
            SetProperty(wstring_to_utf8(propertyName), value);
        }
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
        ///       var i = document.getElementById("plugin")[12];
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
        ///       document.getElementById("plugin")[12] = "property value";
        /// @endcode
        ///
        /// @param  idx     Zero-based index of the property to set the value of.
        /// @param  value   The new property value.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void SetProperty(int idx, const variant& value) = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual variant RemoveProperty(const std::wstring& propertyName)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void RemoveProperty(const std::wstring& propertyName)
        {
            return RemoveProperty(wstring_to_utf8(propertyName));
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual variant RemoveProperty(const std::string& propertyName) = 0
        ///
        /// @brief  Removes a property
        ///
        /// @param  propertyName    Name of the property.
        /// @since 1.5
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void RemoveProperty(const std::string& propertyName) = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual variant RemoveProperty(int idx) = 0
        ///
        /// @brief  Removes an indexed property.
        ///
        /// This can be used for providing array-style access on your object.  For example, the following
        /// will result in a call to RemoveProperty with idx = 12:
        /// @code
        ///       delete document.getElementById("plugin")[12];
        /// @endcode
        ///
        /// @param  idx Zero-based index of the property to remove
        /// @since 1.5
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void RemoveProperty(int idx) = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual variant Invoke(const std::wstring& methodName, const std::vector<variant>& args)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual variant Invoke(const std::wstring& methodName, const std::vector<variant>& args)
        {
            return Invoke(wstring_to_utf8(methodName), args);
        }

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

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual variant Construct(const std::vector<variant>& args) = 0
        ///
        /// @brief  Called by the browser to construct the JSAPI object.
        /// @code
        ///       var x = new MyJSAPIObject();
        /// @endcode
        ///
        /// @param  args        The arguments.
        ///
        /// @return result of constructor call, the new object
        /// @since 1.5
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual variant Construct(const std::vector<variant>& args) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class scoped_zonelock
    ///
    /// @brief  Provides a helper class for locking
    ///
    /// This class will call pushZone on the provided JSAPI object when instantiated and popZone
    /// when it goes out of scope.
    /// @code
    ///      // In the constructor
    ///      // Register protected members
    ///      {
    ///          FB::scoped_zonelock _l(this, SecurityScope_Protected);
    ///          registerMethod("start", make_method(this, &MyPluginAPI::start));
    ///      } // Zone automatically popped off
    ///      // Register private members
    ///      {
    ///          FB::scoped_zonelock _l(this, SecurityScope_Protected);
    ///          registerMethod("getDirectoryListing", make_method(this, &MyPluginAPI::getDirectoryListing));
    ///      } // Zone automatically popped off
    /// @endcode
    ///
    /// @since 1.4a3
    /// @see FB::JSAPI::pushZone
    /// @see FB::JSAPI::popZone
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class scoped_zonelock : boost::noncopyable
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public FB::scoped_zonelock::scoped_zonelock(const JSAPIPtr &api, const SecurityZone& zone)
        ///
        /// @brief  Accepts a FB::JSAPIPtr and pushes the specified security zone to be used
        ///         until this object goes out of scope
        ///
        /// @param  api     const JSAPIPtr&     JSAPI object to lock the zone for
        /// @param  zone    const SecurityZone& Zone to push
        /// @since 1.4a3
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        scoped_zonelock(const JSAPIPtr &api, const SecurityZone& zone)
            : m_api(api.get()), ref(api) {
            lock(zone);
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public   FB::scoped_zonelock::scoped_zonelock(JSAPI* api, const SecurityZone& zone)
        ///
        /// @brief
        ///
        /// @param  api     JSAPI*        JSAPI object to lock the zone for
        /// @param  zone    const SecurityZone& Zone to push
        /// @since 1.4a3
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        scoped_zonelock(JSAPI* api, const SecurityZone& zone) : m_api(api) {
            lock(zone);
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public   FB::scoped_zonelock::~scoped_zonelock()
        ///
        /// @brief   Unlocks/pops the zone
        /// @since 1.4a3
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        ~scoped_zonelock() {
            if (m_api)
                m_api->popZone();
        }
    private:
        void lock(const SecurityZone& zone) const {
            if (m_api)
                m_api->pushZone(zone);
        }
        JSAPI* m_api;
        const FB::JSAPIPtr ref;
    };
};

// There are important conversion routines that require JSObject and JSAPI to both be loaded
#include "JSObject.h"
#endif

