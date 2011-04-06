/**********************************************************\
Original Author: Georg Fritzsche

Created:    November 7, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#pragma once
#ifndef JSAPIAUTO_H
#define JSAPIAUTO_H

#include <deque>
#include <vector>
#include <string>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp> 
#include "JSAPIImpl.h"
#include "MethodConverter.h"
#include "PropertyConverter.h"
#include "Util/typesafe_event.h"

namespace FB {
    FB_FORWARD_PTR(JSFunction);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @fn template<class C, class R> FB::CallMethodFunctor make_method(C* instance, R (C::*function)())
    /// @brief Overload of FB::make_method() for methods taking 0 arguments. 
    ///
    /// This is used with registerMethod() of FB::JSAPIAuto and there are currently overloads for methods
    /// taking up to 10 arguments:
    /// @code
    /// struct X : FB::JSAPIAuto {
    ///     long add(long a, long b) { return a+b; }
    ///     X() { registerMethod("add", FB::make_method(this, &MyClass::add)); }
    /// };
    /// @endcode
    /// @see FB::JSAPIAuto
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  JSAPIAuto
    ///
    /// @brief  JSAPI class with automatic argument type enforcement 
    ///
    /// Most of the time when implementing a JSAPI class for your plugin you will want to extend
    /// JSAPIAuto.  JSAPIAuto implements all abstract functions from JSAPI and provides an automatic
    /// function map and type conversion for methods that you create.
    /// 
    /// For example, to provide a property called "IsFinished" that javascript can access, you need
    /// two methods:
    /// @code
    ///      bool get_IsFinished();
    ///      void set_IsFinished(bool newVal);
    /// @endcode
    /// Then in the constructor of your class that extends JSAPIAuto (we'll call it MyPluginAPI),
    /// you register it like so:
    /// @code
    ///      registerProperty("IsFinished", 
    ///                       make_property(this, 
    ///                                     &MyPluginAPI::get_IsFinished, 
    ///                                     &MyPluginAPI::set_IsFinished));
    /// @endcode
    /// To register a property that is read--only, use the second form:
    /// @code
    ///      registerProperty("IsFinished", make_property(this, &MyPluginAPI::get_IsFinished));
    /// @endcode 
    ///
    /// Similarly, to provide a method called "add" that accepts two integers and returns an integer,
    /// you need one method on your class:
    /// @code
    ///      int add(int a, int b);
    /// @endcode
    /// Then in the constructor of your class that extends JSAPIAuto (still calling it MyPluginAPI),
    /// you register it like so:
    /// @code
    ///      registerMethod("add", make_method(this, &MyPluginAPI::add));
    /// @endcode
    ///         
    /// If arguments are passed that cannot be converted to an int, a javascript exception will be
    /// thrown.
    /// 
    /// @see JSAPI
    /// @see PluginCore
    /// @author Georg Fritzsche
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class JSAPIAuto : public JSAPIImpl
    {
    protected:
        struct Attribute {
            bool operator<(const Attribute& rh) const
            {
                return value < rh.value;
            }
            FB::variant value;
            bool readonly;
        };
        typedef std::map<boost::tuple<std::string, FB::SecurityZone>, FB::JSFunctionPtr> MethodObjectMap;
        typedef std::map<std::string, FB::SecurityZone> ZoneMap;
        typedef std::map<std::string, Attribute> AttributeMap;

    public:
        /// @brief Description is used by ToString().
        JSAPIAuto(const std::string& description = "<JSAPI-Auto Javascript Object>");
        JSAPIAuto(const SecurityZone& securityLevel, const std::string& description = "<JSAPI-Auto Secure Javascript Object>");
        typedef std::deque<SecurityZone> ZoneStack;

        void init();

        virtual ~JSAPIAuto();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public void registerAttribute(const std::string &name, const FB::variant& value, bool readonly = false)
        ///
        /// @brief   Registers an attribute _name_ and sets the value to _value. Optionally read-only
        ///
        /// @param  name        The name (as seen from JavaScript) of the Attribute
        /// @param  value       The value
        /// @param  readonly    True if the attribute should not be modifiable from JavaScript
        ///
        /// @since 1.4a3
        /// @see getAttribute
        /// @see setAttribute
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void registerAttribute(const std::string &name, const FB::variant& value, bool readonly = false);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public void setReserved(const std::string &name)
        ///
        /// @brief   Prevents attributes from being created from JavaScript with the specified name
        ///
        /// This is generally only useful to prevent your plugin from overriding some attribute on the
        /// object tag it resides in in the DOM.  Attributes such as "id", "name", and "width" are marked
        /// as reserved by default.
        /// 
        /// @param  name    The name of the attribute to mark as reserved
        ///
        /// @returns void
        ///
        /// @since 1.4a3
        /// @see registerAttribute
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setReserved(const std::string &name)
        {
            m_reservedMembers.insert(name);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public bool isReserved( const std::string& propertyName ) const
        ///
        /// @brief   Returns true if the specified propertyName is a reserved attribute
        ///
        /// @param   propertyName    Name of the attribute to check
        ///
        /// @returns bool true if propertyName is reserved
        ///
        /// @since 1.4a3
        /// @see setReserved
        /// @see registerAttribute
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        bool isReserved( const std::string& propertyName ) const
        {
            return m_reservedMembers.find(propertyName) != m_reservedMembers.end();
        }

        virtual void getMemberNames(std::vector<std::string> &nameVector) const;
        virtual size_t getMemberCount() const;

        virtual variant Invoke(const std::string& methodName, const std::vector<variant>& args);
        virtual variant Construct(const std::vector<variant>& args);
        virtual JSAPIPtr GetMethodObject(const std::string& methodObjName);

        virtual void unregisterMethod(const std::wstring& name)
        {
            unregisterMethod(FB::wstring_to_utf8(name));
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void JSAPIAuto::unregisterMethod(const std::string& name)
        ///
        /// @brief  Unregisters a method that has been exposed to javascript 
        ///
        /// This is the opposite of registerMethod
        ///
        /// @param  name    The name that the method will have when accessed from javascript. 
        /// @see registerMethod
        /// @since 1.4b4
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void unregisterMethod(const std::string& name);

        virtual void registerMethod(const std::wstring& name, const CallMethodFunctor& func)
        {
            registerMethod(FB::wstring_to_utf8(name), func);
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void JSAPIAuto::registerMethod(const std::string& name, const CallMethodFunctor& func)
        ///
        /// @brief  Registers a method to be exposed to javascript 
        ///
        /// To provide a method to javascript called "add" that accepts two integers and returns an integer,
        /// you need one method on your class:
        /// @code
        ///      int add(int a, int b);
        /// @endcode
        /// Then in the constructor of your class that extends JSAPIAuto (still calling it MyPluginAPI),
        /// you register it like so:
        /// @code
        ///      registerMethod("add", make_method(this, &MyPluginAPI::add));
        /// @endcode
        ///         
        /// You should then be able to call the method from javascript and get a result:
        /// @code
        ///      // Assumes that this JSAPI object is the Root JSAPI for the plugin
        ///      alert(document.getElementById("plugin").add(2,765));
        /// @endcode
        ///
        /// @param  name    The name that the method will have when accessed from javascript. 
        /// @param  func    The result of a make_method call given the class instance and function ptr
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void registerMethod(const std::string& name, const CallMethodFunctor& func);

        virtual bool HasMethod(const std::string& methodName) const;
        virtual bool HasMethodObject(const std::string& methodObjName) const;
        virtual bool HasProperty(const std::string& propertyName) const;
        virtual bool HasProperty(int idx) const;

        virtual void registerProperty(const std::wstring& name, const PropertyFunctors& propFuncs);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void JSAPIAuto::registerProperty(const std::string& name,
        /// const PropertyFunctors& propFuncs)
        ///
        /// @brief  Register property to be exposed to javascript. 
        ///
        /// To provide a property called "IsFinished" that javascript can access, you need
        /// two methods:
        /// @code
        ///      bool get_IsFinished();
        ///      void set_IsFinished(bool newVal);
        /// @endcode
        /// Then in the constructor of your class that extends JSAPIAuto (we'll call it MyPluginAPI),
        /// you register it like so:
        /// @code
        ///      registerProperty("IsFinished", make_property(this, &MyPluginAPI::get_IsFinished, &MyPluginAPI::set_IsFinished));
        /// @endcode
        /// To register a property that is read- or write-only, simply provide "NULL" for the getter or
        /// setter function when registering the property.
        ///
        /// @param  name        The name. 
        /// @param  propFuncs   The property funcs. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void registerProperty(const std::string& name, const PropertyFunctors& propFuncs);

        virtual void unregisterProperty(const std::wstring& name);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void JSAPIAuto::unregisterProperty(const std::string& name)
        ///
        /// @brief  Unregisters a property that has been exposed to javascript 
        ///
        /// This is the opposite of registerProperty
        ///
        /// @param  name    The name that the method will have when accessed from javascript. 
        /// @see registerProperty
        /// @since 1.4b4
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void unregisterProperty(const std::string& name);
        
        virtual variant GetProperty(const std::string& propertyName);
        virtual void SetProperty(const std::string& propertyName, const variant& value);
        virtual void RemoveProperty(const std::string& propertyName);
        virtual variant GetProperty(int idx);
        virtual void SetProperty(int idx, const variant& value);
        virtual void RemoveProperty(int idx);

        virtual void FireJSEvent(const std::string& eventName, const FB::VariantMap &members, const FB::VariantList &arguments);
        virtual void fireAsyncEvent( const std::string& eventName, const std::vector<variant>& args );

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual std::string JSAPIAuto::ToString()
        ///
        /// @brief  Default method called when a string value is requested for the scriptable object.
        ///
        /// Override this to provide your own, otherwise it returns the description passed to the constructor.
        ///         
        /// @return A string representation of this object. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::string ToString()
        {
            return m_description;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool JSAPIAuto::get_valid()
        ///
        /// @brief  Property exposed by default to javascript useful for checking to make sure that the
        ///         JSAPI is working.
        ///
        /// @return true if it succeeds, false if it fails. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool get_valid()
        {
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public virtual FB::variant getAttribute(const std::string& name)
        ///
        /// @brief   Returns the attribute with the given name, empty variant if none
        ///
        /// @param  name    name of the attribute to get
        ///
        /// @returns FB::variant
        ///
        /// @since 1.4a3
        /// @see registerAttribute
        /// @see setAttribute
        /// @see removeAttribute
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual FB::variant getAttribute(const std::string& name);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public virtual void setAttribute(const std::string& name, const FB::variant& value)
        ///
        /// @brief   Assigns a value to the specified attribute, if it is not reserved or read-only
        ///
        /// @param  name    name of the attribute to set
        /// @param  value   value for the attribute
        ///
        /// @since 1.4a3
        /// @see registerAttribute
        /// @see getAttribute
        /// @see removeAttribute
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setAttribute(const std::string& name, const FB::variant& value);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public virtual void removeAttribute(const std::string& name)
        ///
        /// @brief   Removes the attribute with the given name
        ///
        /// @param  name    name of the attribute to remove
        ///
        /// @since 1.5
        /// @see registerAttribute
        /// @see setAttribute
        /// @see getAttribute
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void unregisterAttribute(const std::string& name);

    protected:
        bool memberAccessible( ZoneMap::const_iterator it ) const
        {
            return (it != m_zoneMap.end()) && getZone() >= it->second;
        }

    protected:
        // Stores Method Objects -- JSAPI proxy objects for calling a method on this object
        MethodObjectMap m_methodObjectMap;
        // Stores the methods exposed to JS
        MethodFunctorMap m_methodFunctorMap;
        // Stores the properties exposed to JS
        PropertyFunctorsMap m_propertyFunctorsMap;
        // Keeps track of the security zone of each member
        ZoneMap m_zoneMap;
        
        const std::string m_description;

        AttributeMap m_attributes;
        FB::StringSet m_reservedMembers;
        bool m_allowDynamicAttributes;
        bool m_allowRemoveProperties;
        bool m_allowMethodObjects;
    public:
        static bool s_allowDynamicAttributes;
        static bool s_allowRemoveProperties;
        static bool s_allowMethodObjects;
    };

};

#endif // JSAPIAUTO_H

