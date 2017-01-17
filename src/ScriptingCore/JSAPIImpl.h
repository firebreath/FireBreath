/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    April 8, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html
            
Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_JSAPIImpl
#define H_FB_JSAPIImpl

#include "APITypes.h"
#include <list>
#include <deque>
#include <boost/noncopyable.hpp>
#include "JSExceptions.h"
#include "JSAPI.h"

namespace FB
{
    FB_FORWARD_PTR(JSAPIImpl);

	/// @brief  Defines an alias representing a method functor used by FB::JSAPIAuto, created by FB::make_method().
	using CallMethodFunctor = std::function < variantPromise(const std::vector<variant>&) >;
	struct MethodFunctors
	{
		FB::CallMethodFunctor call;
		SecurityZone zone;
		MethodFunctors() : call() {}
		MethodFunctors(const CallMethodFunctor& call) : call(call) {}
		MethodFunctors(const SecurityZone& zone, const CallMethodFunctor& call) : call(call), zone(zone) {}
		MethodFunctors(const MethodFunctors& m) : call(m.call) {}
		MethodFunctors& operator=(const MethodFunctors& rhs) {
			call = rhs.call;
			zone = rhs.zone;
			return *this;
		}
	};
	/// @brief  Defines an alias representing a map of method functors used by FB::JSAPIAuto
	using MethodFunctorMap = std::map < std::string, MethodFunctors >;
	
	// new style JSAPI properties

	/// @brief  Defines an alias representing a property getter functor used by FB::JSAPIAuto
	using GetPropFunctor = std::function < variantPromise() >;
	/// @brief  Defines an alias representing a property setter functor used by FB::JSAPIAuto
	using SetPropFunctor = std::function < void(const FB::variant&) >;
	/// @brief  used by FB::JSAPIAuto to store property implementation details, created by FB::make_property().
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
	/// @brief  Defines an alias representing a map of property functors used by FB::JSAPIAuto
	using PropertyFunctorsMap = std::map < std::string, PropertyFunctors >;

	// JSAPI event handlers

	using EventPair = std::pair < std::string, FB::JSObjectPtr >;
	using EventMultiMap = std::multimap < std::string, FB::JSObjectPtr >;
	using EventIFaceMap = std::map < void*, FB::JSObjectPtr >;
	using EventSingleMap = std::map < std::string, FB::JSObjectPtr >;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  JSAPIImpl
    ///
    /// @brief  JavaScript API base class implementation -- provides basic functionality for C++ JSAPI
    ///         objects.
    /// 
    /// JSAPIImpl is the base class for C++ objects that interact with Javascript. It provides some basic
    /// functionality that is shared with most C++ JSAPI implementations. If you want to provide an
    /// interface to something else, such as an object from Javascript, you probably want to extend
    /// JSAPI instead so as not to get the extra (unused) functionality from this class
    ///
    /// @author Richard Bateman
    /// @see FB::JSAPI
    /// @see FB::JSAPIAuto
    /// @since 1.5
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class JSAPIImpl : public JSAPI
    {
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn JSAPIImpl(void)
        ///
        /// @brief  Default constructor. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        JSAPIImpl(void);
        JSAPIImpl( const SecurityZone& securityLevel );

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual ~JSAPIImpl(void)
        ///
        /// @brief  Finaliser. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~JSAPIImpl(void);

    public:
        JSAPIImplPtr shared_from_this() { return std::static_pointer_cast<JSAPIImpl>(JSAPI::shared_from_this()); }
        std::shared_ptr<const JSAPIImpl> shared_from_this() const { return std::static_pointer_cast<const JSAPIImpl>(JSAPI::shared_from_this()); }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn void invalidate()
        ///
        /// @brief  Invalidates this object.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void invalidate();
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn void shutdown()
        ///
        /// @brief  Called to notify the object that the plugin is shutting down
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void shutdown() {};

    protected:
        virtual void fireAsyncEvent( std::string eventName, const std::vector<variant>& args );

    protected:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual void FireEvent(const std::wstring& eventName, const std::vector<variant> &args)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void FireEvent(const std::wstring& eventName, const std::vector<variant> &args)
        {
            FireEvent(wstring_to_utf8(eventName), args);
        }
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void FireEvent(std::string eventName, const std::vector<variant> &args)
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
        ///      // Note that the convention used by these browsers is that "on" is implied
        ///      document.getElementByID("plugin").addEventListener("load", function() { alert("loaded!"); }, false);;/.
        /// @endcode
        ///
        /// You can then fire the event -- from any thread -- from the JSAPI object like so:
        /// @code
        ///      FireEvent("onload", FB::VariantList{"param1", 2, 3.0});
        /// @endcode
        ///         
        /// Also note that registerEvent must be called from the constructor to register the event.
        /// @code
        ///      registerEvent("onload");
        /// @endcode
        /// 
        /// @param  eventName   Name of the event.  This event must start with "on"
        /// @param  args        The arguments that should be sent to each attached event handler
        ///
        /// @see registerEvent
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void FireEvent(std::string eventName, const std::vector<variant> &args);
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief  Fires an event into javascript asynchronously using a W3C-compliant event parameter
        ///         
        /// This fires an event to all handlers attached to the given event in javascript. With a
        /// W3C-compliant event parameter
        /// 
        /// IE:
        /// @code
        ///      document.getElementByID("plugin").attachEvent("onload", function() { alert("loaded!"); });
        /// @endcode
        /// Firefox/Safari/Chrome/Opera:
        /// @code
        ///      // Note that the convention used by these browsers is that "on" is implied
        ///      document.getElementByID("plugin").addEventListener("load", function() { alert("loaded!"); }, false);;/.
        /// @endcode
        ///
        /// You can then fire the event -- from any thread -- from the JSAPI object like so:
        /// @code
        ///      FireEvent("onload", FB::VariantList{"param1", 2, 3.0});
        /// @endcode
        ///         
        /// Also note that registerEvent must be called from the constructor to register the event.
        /// @code
        ///      registerEvent("onload");
        /// @endcode
        /// 
        /// @param  eventName   Name of the event.  This event must start with "on"
        /// @param  members
        /// @param  arguments        The arguments that should be sent to each attached event handler
        ///
        /// @see registerEvent
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void FireJSEvent(std::string eventName, const FB::VariantMap &members, const FB::VariantList &arguments);
        /// @overload
        virtual void FireJSEvent(std::string eventName, const FB::VariantMap &params)
        {
            FireJSEvent(eventName, params, FB::VariantList());
        }
        /// @overload
        virtual void FireJSEvent(std::string eventName, const FB::VariantList &arguments)
        {
            FireJSEvent(eventName, FB::VariantMap(), arguments);
        }

    public:
        virtual void pushZone(const SecurityZone& securityLevel)
        {
            m_zoneMutex.lock();
            m_zoneStack.push_back(securityLevel);
        }

        virtual void popZone()
        {
            m_zoneStack.pop_back();
            m_zoneMutex.unlock();
        }

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
        virtual void setDefaultZone(const SecurityZone& securityLevel)
        {
            std::unique_lock<std::recursive_mutex> lock(m_zoneMutex);
            assert(!m_zoneStack.empty());
            m_zoneStack.pop_front();
            m_zoneStack.push_front(securityLevel);
        }

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
        virtual SecurityZone getDefaultZone() const
        {
            std::unique_lock<std::recursive_mutex> lock(m_zoneMutex);
            assert(!m_zoneStack.empty());
            return m_zoneStack.front();
        }

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
        virtual SecurityZone getZone() const
        {
            assert(!m_zoneStack.empty());
            std::unique_lock<std::recursive_mutex> lock(m_zoneMutex);
            return m_zoneStack.back();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void registerEvent(std::string name)
        ///
        /// @brief  Register event so that event listeners can be added/attached from javascript
        ///
        /// @param  name    The name of the event to register.  This event must start with "on"
        /// @deprecated 1.5
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void registerEvent(std::string name) {}
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual void registerEvent(const std::wstring& name)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void registerEvent(const std::wstring& name)
        {
            registerEvent(wstring_to_utf8(name));
        }


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void registerEventMethod(std::string name, JSObjectPtr& event)
        ///
        /// @brief  Called by the browser to register an event handler method
        ///
        /// @param  name            The name. 
        /// @param  event           The event handler method. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void registerEventMethod(std::string name, JSObjectPtr& event);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual void registerEventMethod(const std::wstring& name, JSObjectPtr& event)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void registerEventMethod(const std::wstring& name, JSObjectPtr& event)
        {
            registerEventMethod(wstring_to_utf8(name), event);
        }
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void unregisterEventMethod(std::string name, JSObjectPtr& event)
        ///
        /// @brief  Called by the browser to unregister an event handler method
        ///
        /// @param  name            The name. 
        /// @param  event           The event handler method to unregister. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void unregisterEventMethod(std::string name, JSObjectPtr& event);
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
            std::unique_lock<std::recursive_mutex> _l(m_eventMutex);
            m_evtIfaces[event->getEventContext()][static_cast<void*>(event.get())] = event;
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
            std::unique_lock<std::recursive_mutex> _l(m_eventMutex);
            EventIFaceMap::iterator fnd = m_evtIfaces[event->getEventContext()].find(static_cast<void*>(event.get()));
            m_evtIfaces[event->getEventContext()].erase(fnd);
        }

    public:
        virtual void registerProxy(const JSAPIImplWeakPtr &ptr) const;
        virtual void unregisterProxy(const FB::JSAPIImplPtr& ptr) const;

    protected:
        using ZoneStack = std::deque < SecurityZone > ;
        using EventContextMap = std::map < void*, EventMultiMap > ;
        // Stores event handlers
        EventContextMap m_eventMap;
        using EventIfaceContextMap = std::map < void*, EventIFaceMap > ;
        // Stores event interfaces
        EventIfaceContextMap m_evtIfaces;

        using ProxyList = std::vector < JSAPIImplWeakPtr > ;
        mutable ProxyList m_proxies;

        mutable std::recursive_mutex m_eventMutex;
        mutable std::recursive_mutex m_proxyMutex;
        mutable std::recursive_mutex m_zoneMutex;
        ZoneStack m_zoneStack;
                
        bool m_valid;                   // Tracks if this object has been invalidated
    };
};

// There are important conversion routines that require JSObject and JSAPI to both be loaded
#include "JSObject.h"
#endif

