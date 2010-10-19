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
#ifndef H_FB_APITYPES
#define H_FB_APITYPES

#include <string>
#include <vector>
#include <map>
#include <set>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

// get rid of "unused variable" warnings
#define FB_UNUSED_VARIABLE(x) ((void)(x))

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @namespace  FB
///
/// @brief  Primary location of FireBreath classes and utility functions.
/// 		
/// The five most important classes to understand when implementing a FireBreath plugin are:
///   - FB::PluginCore
///   - FB::JSAPI / FB::JSAPIAuto
///   - FB::BrowserHost
///   - FB::JSObject
///   - FB::variant
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace FB
{
    class BrowserHost;
    class JSAPI;
    class JSObject;
    class variant;
    
    // Variant list

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @typedef    FB::VariantList
    ///
    /// @brief  Defines an alias representing list of variants.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef std::vector<variant> VariantList;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @typedef    FB::VariantMap
    ///
    /// @brief  Defines an alias representing a string -> variant map.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef std::map<std::string, variant> VariantMap;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @typedef    FB::StringSet
    ///
    /// @brief  Defines an alias representing a set of std::strings
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef std::set<std::string> StringSet;
    
    // FB pointer types

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @typedef    FB::JSAPIPtr
    ///
    /// @brief  Defines an alias for a JSAPI shared_ptr (you should never use a JSAPI* directly)
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef boost::shared_ptr<FB::JSAPI> JSAPIPtr; 

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @typedef    FB::JSObjectPtr
    ///
    /// @brief  Defines an alias representing a JSObject shared_ptr (you should never use a 
    /// 		JSObject* directly)
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef boost::shared_ptr<FB::JSObject> JSObjectPtr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @typedef    FB::BrowserHostPtr
    ///
    /// @brief  Defines an alias representing a BrowserHost shared_ptr (you should never use a 
    /// 		BrowserHost* directly)
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef boost::shared_ptr<FB::BrowserHost> BrowserHostPtr;
    
    // backwards compability typedefs

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @typedef    FB::BrowserHostWrapper
    ///
    /// @brief  Defines a alias for backwards compatibility
    /// @deprecated 1.3
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef BrowserHost BrowserHostWrapper;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @typedef    FB::BrowserObjectAPI
    ///
    /// @brief  Defines a alias for backwards compatibility
    /// @deprecated 1.3
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef JSObject BrowserObjectAPI;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @typedef    FB::JSOutObject
    ///
    /// @brief  Defines an alias for JSOutObject -> JSAPIPtr
    /// @deprecated 1.2
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef JSAPIPtr JSOutObject;
    
    // deprecation warnings
    
#if defined(_MSC_VER)
#  pragma deprecated(BrowserHostWrapper, BrowserObjectAPI, JSOutObject)
#elif defined(__GNUC__)
    typedef BrowserHost BrowserHostWrapper __attribute__((deprecated));
    typedef JSObject BrowserObjectAPI __attribute__((deprecated));
    typedef JSAPIPtr JSOutObject __attribute__((deprecated));
#endif
    
    // dynamically cast a FB pointer

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @fn template<class T, class U> boost::shared_ptr<T> ptr_cast(boost::shared_ptr<U> const & r)
    ///
    /// @brief  Convenience function for doing a dynamic cast of one boost::shared_ptr to another
    ///
    /// This is simply an alias for boost::dynamic_ptr_cast<T>
    /// 
    /// @param  r   The value to cast
    ///
    /// @return A boost::shared_ptr<T>; if the dynamic cast failed this will be empty
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    template<class T, class U> 
    boost::shared_ptr<T> ptr_cast(boost::shared_ptr<U> const & r);

    /// @brief  Defines an alias representing a function pointer to JSAPI::Invoke
    typedef variant (JSAPI::*InvokeType)(const std::string&, const std::vector<variant>&);
    /// @brief  Defines an alias representing a function pointer to JSAPI::SetProperty
    typedef void (JSAPI::*SetPropertyType)(const std::string&, const variant&);
    /// @brief  Defines an alias representing a function pointer to JSAPI::GetProperty
    typedef variant (JSAPI::*GetPropertyType)(const std::string&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @struct CatchAll
    ///
    /// @brief  When used as a parameter on a JSAPIAuto function this matches 0 or more variants
    /// 		-- in other words, all other parameters from this point on regardless of type.
    ///
    /// @author Richard Bateman
    /// @date   10/15/2010
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    struct CatchAll {
        typedef FB::VariantList value_type;
        FB::VariantList value;
    };

    // JSAPI methods

    class JSAPI;
    /// @brief  Defines an alias representing a function ptr for a method on a FB::JSAPISimple object
    typedef variant (JSAPI::*CallMethodPtr)(const std::vector<variant>&);
    /// @brief Used by FB::JSAPISimple to store information about a method
    struct MethodInfo {
        MethodInfo() : callFunc(NULL) { }
        MethodInfo(CallMethodPtr callFunc) : callFunc(callFunc) { }
        MethodInfo(const MethodInfo &rh) : callFunc(rh.callFunc) { }
        CallMethodPtr callFunc;
    };

    /// @brief  Defines an alias representing a map of methods used by FB::JSAPISimple
    typedef std::map<std::string, MethodInfo> MethodMap;

    // JSAPI properties


    /// @brief  Defines an alias representing a function pointer for a property getter on a FB::JSAPISimple object
    typedef variant (JSAPI::*GetPropPtr)();
    /// @brief  Defines an alias representing a function pointer for a property setter on a FB::JSAPISimple object
    typedef void (JSAPI::*SetPropPtr)(const variant& value);
    /// @brief Used by FB::JSAPISimple to store information about a property
    struct PropertyInfo {
        PropertyInfo() : getFunc(NULL), setFunc(NULL) { }
        PropertyInfo(GetPropPtr getFunc, SetPropPtr setFunc) : getFunc(getFunc), setFunc(setFunc) { }
        PropertyInfo(const PropertyInfo &rh) : getFunc(rh.getFunc), setFunc(rh.setFunc) { }
        GetPropPtr getFunc;
        SetPropPtr setFunc;
    };

    /// @brief  Defines an alias representing a map of properties used by FB::JSAPISimple
    typedef std::map<std::string, PropertyInfo> PropertyMap;

    // new style JSAPI methods

    /// @brief  Defines an alias representing a method functor used by FB::JSAPIAuto
    typedef boost::function<variant (const std::vector<variant>&)> CallMethodFunctor;
    /// @brief  Defines an alias representing a map of method functors used by FB::JSAPIAuto
    typedef std::map<std::string, CallMethodFunctor> MethodFunctorMap;

    // new style JSAPI properties

    /// @brief  Defines an alias representing a property getter functor used by FB::JSAPIAuto
    typedef boost::function<FB::variant ()> GetPropFunctor;
    /// @brief  Defines an alias representing a property setter functor used by FB::JSAPIAuto
    typedef boost::function<void (const FB::variant&)> SetPropFunctor;
    /// @brief  used by FB::JSAPIAuto to store property implementation details
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
    typedef std::map<std::string, PropertyFunctors> PropertyFunctorsMap;

    // JSAPI event handlers

    typedef std::pair<std::string, FB::JSObjectPtr> EventPair;
    typedef std::multimap<std::string, FB::JSObjectPtr> EventMultiMap;
    typedef std::map<void*, FB::JSObjectPtr> EventIFaceMap;
    typedef std::map<std::string, FB::JSObjectPtr> EventSingleMap;
    
    // implementation details
    
    template<class T, class U> 
    boost::shared_ptr<T> ptr_cast(boost::shared_ptr<U> const & r) 
    {
        return boost::dynamic_pointer_cast<T>(r);
    }
}

// This needs to be included after all our classes are defined because it relies on types defined in this file
// TODO: can this be done better?
#include "variant.h"

#endif
