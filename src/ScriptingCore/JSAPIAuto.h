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

#include "JSAPI.h"
#include "MethodConverter.h"
#include "PropertyConverter.h"
#include <vector>
#include <string>

namespace FB {
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
    class JSAPIAuto : public JSAPI
    {
    public:
        /// @brief Description is used by ToString().
        JSAPIAuto(const std::string& description = "<JSAPI-Auto driven Javascript Object>");
        virtual ~JSAPIAuto();

        virtual void registerAttribute(const std::string &name, const FB::variant& value, bool readonly = false);
        virtual void setReserved(const std::string &name);

        virtual void getMemberNames(std::vector<std::string> &nameVector);
        virtual size_t getMemberCount();

        virtual variant Invoke(const std::string& methodName, const std::vector<variant>& args);

        virtual void registerMethod(const std::wstring& name, const CallMethodFunctor& func);
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

        virtual bool HasMethod(const std::string& methodName);
        virtual bool HasProperty(const std::string& propertyName);
        virtual bool HasProperty(int idx);

        bool isReserved( const std::string& propertyName );


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
        
        virtual variant GetProperty(const std::string& propertyName);
        virtual void SetProperty(const std::string& propertyName, const variant& value);
        virtual variant GetProperty(int idx);
        virtual void SetProperty(int idx, const variant& value);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual std::string JSAPIAuto::ToString()
        ///
        /// @brief  Default method called when a string value is requested for the scriptable object.
        ///
        /// Override this to provide your own, otherwise it returns the description passed to the constructor.
        ///         
        /// @return A string representation of this object. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::string ToString();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool JSAPIAuto::get_valid()
        ///
        /// @brief  Property exposed by default to javascript useful for checking to make sure that the
        ///         JSAPI is working.
        ///
        /// @return true if it succeeds, false if it fails. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool get_valid();

        virtual FB::variant getAttribute(const std::string& name);
        virtual void setAttribute(const std::string& name, const FB::variant& value);
    protected:
        MethodFunctorMap m_methodFunctorMap;        // Stores the methods exposed to JS
        PropertyFunctorsMap m_propertyFunctorsMap;  // Stores the properties exposed to JS
        
        const std::string m_description;

        struct Attribute {
            bool operator<(const Attribute& rh) const
            {
                return value < rh.value;
            }
            FB::variant value;
            bool readonly;
        };
        typedef std::map<std::string, Attribute> AttributeMap;

        AttributeMap m_attributes;
        FB::StringSet m_reservedMembers;
        bool m_allowDynamicAttributes;
    };
};

#endif // JSAPIAUTO_H
