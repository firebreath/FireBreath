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
#include <boost/variant/variant_fwd.hpp>
#include "fb_stdint.h"
#include "FBPointers.h"

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
    FB_FORWARD_PTR(BrowserHost);
    FB_FORWARD_PTR(JSAPI);
    FB_FORWARD_PTR(JSObject);
    class variant;
    namespace variant_detail {
        // Note that null translates to returning NULL
        struct null;
        // Note that empty translates into return VOID (undefined)
        struct empty;
    }
    
    // Variant list

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @typedef    FB::VariantList
    ///
    /// @brief  Defines an alias representing list of variants.
    /// @see FB::variant_list_of()
    /// @see FB::make_variant_list()
    /// @see FB::convert_variant_list()
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef std::vector<variant> VariantList;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @typedef    FB::VariantMap
    ///
    /// @brief  Defines an alias representing a string -> variant map.
    /// @see FB::variant_map_of()
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
    /// @typedef    FB::JSAPIWeakPtr
    ///
    /// @brief  Defines an alias for a JSAPI weak_ptr (you should never use a JSAPI* directly)
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef boost::weak_ptr<FB::JSAPI> JSAPIWeakPtr; 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @typedef    FB::JSAPIPtr
    ///
    /// @brief  Defines an alias for a JSAPI shared_ptr (you should never use a JSAPI* directly)
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef boost::shared_ptr<FB::JSAPI> JSAPIPtr; 

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @typedef    FB::JSObjectWeakPtr
    ///
    /// @brief  Defines an alias representing a JSObject weak_ptr (you should never use a 
    ///         JSObject* directly)
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef boost::weak_ptr<FB::JSObject> JSObjectWeakPtr;
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @typedef    FB::JSObjectPtr
    ///
    /// @brief  Defines an alias representing a JSObject shared_ptr (you should never use a 
    ///         JSObject* directly)
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef boost::shared_ptr<FB::JSObject> JSObjectPtr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @typedef    FB::BrowserHostPtr
    ///
    /// @brief  Defines an alias representing a BrowserHost shared_ptr (you should never use a 
    ///         BrowserHost* directly)
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
    /// @brief  Defines an alias representing a function pointer to JSAPI::Invoke
    typedef variant (JSAPI::*ConstructType)(const std::vector<variant>&);
    /// @brief  Defines an alias representing a function pointer to JSAPI::SetProperty
    typedef void (JSAPI::*SetPropertyType)(const std::string&, const variant&);
    /// @brief  Defines an alias representing a function pointer to JSAPI::GetProperty
    typedef variant (JSAPI::*GetPropertyType)(const std::string&);
    /// @brief  Defines an alias representing a function pointer to JSAPI::GetProperty
    typedef void (JSAPI::*RemovePropertyType)(const std::string&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @struct CatchAll
    ///
    /// @brief  When used as a parameter on a JSAPIAuto function this matches 0 or more variants
    ///         -- in other words, all other parameters from this point on regardless of type.
    /// 
    /// This helper struct allows your scriptable methods to receive 0 or more parameters in addition
    /// to some fixed ones. E.g. given the following scriptable method:
    /// @code
    /// long howManyParams(long a, const std::string& b, const FB::CatchAll& more) {
    ///     const FB::VariantList& values = more.value;
    ///     long paramCount = 2 + values.size();
    ///     return paramCount;
    /// }
    /// @endcode
    /// The following calls would result in:
    /// @code
    /// > obj.howManyParams(42, "moo");
    /// => returns 2
    /// > obj.howManyParams(42, "moo", 1.0, "meh");
    /// => returns 4
    /// @endcode
    ///
    /// @author Georg Fritzsche
    /// @date   10/15/2010
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    struct CatchAll {
        typedef FB::VariantList value_type;
        FB::VariantList value;
    };

    // Special Variant types
    typedef FB::variant_detail::empty FBVoid;
    typedef FB::variant_detail::null FBNull;
    struct FBDateString {
    public:
        FBDateString() { }
        FBDateString(const FBDateString& rhs) : date(rhs.date) { }
        FBDateString(const std::string &dstr) : date(dstr) { }

        FBDateString& operator=(const std::string& dstr) { date = dstr; return *this; }
        std::string getValue() { return date; }
        void setValue(std::string value) { date = value; }
        bool operator<(const std::string& rh) const
        {
            return date < rh;
        }
        bool operator<(const FBDateString& rh) const
        {
            return date < rh.date;
        }
        
    protected:
        std::string date;
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
    /// @brief  Used to set a SecurityZone for a method or property -- used by JSAPIAuto
    typedef int SecurityZone;

    /// @brief  Default SecurityZone values; you can use these or provide your own
    enum SecurityLevel {
        SecurityScope_Public = 0,
        SecurityScope_Protected = 2,
        SecurityScope_Private = 4,
        SecurityScope_Local = 6
    };

    /// @brief  Defines an alias representing a method functor used by FB::JSAPIAuto, created by FB::make_method().
    typedef boost::function<variant (const std::vector<variant>&)> CallMethodFunctor;
    struct MethodFunctors
    {
        CallMethodFunctor call;
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
    typedef std::map<std::string, MethodFunctors> MethodFunctorMap;

    // new style JSAPI properties

    /// @brief  Defines an alias representing a property getter functor used by FB::JSAPIAuto
    typedef boost::function<FB::variant ()> GetPropFunctor;
    /// @brief  Defines an alias representing a property setter functor used by FB::JSAPIAuto
    typedef boost::function<void (const FB::variant&)> SetPropFunctor;
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

    namespace boost_variant {
        typedef boost::variant<long, int, double, std::string, FB::JSAPIPtr, FB::JSObjectPtr, FB::FBNull, FB::FBVoid> fb_compat;
        typedef boost::variant<long, int, double, float, std::string, FB::FBNull, FB::FBVoid> primitives;
        typedef boost::variant<std::string, FB::StringSet> strings;
    }

    struct Rect {
        int32_t top;
        int32_t left;
        int32_t bottom;
        int32_t right;
    };
}

// This needs to be included after all our classes are defined because it relies on types defined in this file
// TODO: can this be done better?
#include "variant.h"

#endif

