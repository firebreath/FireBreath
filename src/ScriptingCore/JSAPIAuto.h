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
#include <tuple>
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
        using ZoneMap = std::map<std::string, FB::SecurityZone>;
        using AttributeMap = std::map<std::string, Attribute>;

    public:
        /// @brief Description is used by ToString().
        JSAPIAuto(std::string description = "<JSAPI-Auto Javascript Object>");
        JSAPIAuto(const SecurityZone& securityLevel, std::string description = "<JSAPI-Auto Secure Javascript Object>");
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

        virtual void getMemberNames(std::vector<std::string> &nameVector) const override;
        virtual size_t getMemberCount() const override;

        virtual variantPromise Invoke(std::string methodName, const std::vector<variant>& args) override;

        virtual void unregisterMethod(const std::wstring& name)
        {
            unregisterMethod(FB::wstring_to_utf8(name));
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void JSAPIAuto::unregisterMethod(std::string name)
        ///
        /// @brief  Unregisters a method that has been exposed to javascript 
        ///
        /// This is the opposite of registerMethod
        ///
        /// @param  name    The name that the method will have when accessed from javascript. 
        /// @see registerMethod
        /// @since 1.4b4
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void unregisterMethod(std::string name);

        virtual void registerMethod(const std::wstring& name, const CallMethodFunctor& func)
        {
            registerMethod(FB::wstring_to_utf8(name), func);
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void JSAPIAuto::registerMethod(std::string name, const CallMethodFunctor& func)
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
        virtual void registerMethod(std::string name, const CallMethodFunctor& func);

        virtual bool HasMethod(std::string methodName) const override;
        virtual bool HasProperty(std::string propertyName) const override;
        virtual bool HasProperty(int idx) const override;

        virtual void registerProperty(const std::wstring& name, const PropertyFunctors& propFuncs);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void JSAPIAuto::registerProperty(std::string name,
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
        virtual void registerProperty(std::string name, const PropertyFunctors& propFuncs);

        virtual void unregisterProperty(const std::wstring& name);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void JSAPIAuto::unregisterProperty(std::string name)
        ///
        /// @brief  Unregisters a property that has been exposed to javascript 
        ///
        /// This is the opposite of registerProperty
        ///
        /// @param  name    The name that the method will have when accessed from javascript. 
        /// @see registerProperty
        /// @since 1.4b4
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void unregisterProperty(std::string name);
        
        virtual variantPromise GetProperty(std::string propertyName) override;
        virtual void SetProperty(std::string propertyName, const variant& value) override;
        virtual void RemoveProperty(std::string propertyName) override;
        virtual variantPromise GetProperty(int idx) override;
        virtual void SetProperty(int idx, const variant& value) override;
        virtual void RemoveProperty(int idx) override;

        virtual void FireJSEvent(std::string eventName, const FB::VariantMap &members, const FB::VariantList &arguments) override;
        virtual void fireAsyncEvent( std::string eventName, const std::vector<variant>& args ) override;

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
        /// @fn public virtual FB::variant getAttribute(std::string name)
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
        virtual FB::variantPromise getAttribute(std::string name);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public virtual void setAttribute(std::string name, const FB::variant& value)
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
        virtual void setAttribute(std::string name, const FB::variant& value);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public virtual void removeAttribute(std::string name)
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
        virtual void unregisterAttribute(std::string name);

    protected:
        bool memberAccessible( ZoneMap::const_iterator it ) const
        {
            return (it != m_zoneMap.end()) && getZone() >= it->second;
        }

    protected:
        // Stores the methods exposed to JS
        MethodFunctorMap m_methodFunctorMap;
        // Stores the properties exposed to JS
        PropertyFunctorsMap m_propertyFunctorsMap;
        // Keeps track of the security zone of each member
        ZoneMap m_zoneMap;
        
        const std::string m_description;

        AttributeMap m_attributes;
        bool m_allowRemoveProperties;
    public:
        static bool s_allowRemoveProperties;
    };

};

#endif // JSAPIAUTO_H

