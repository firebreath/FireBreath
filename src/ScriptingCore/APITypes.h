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

#ifndef H_FB_APITYPES
#define H_FB_APITYPES

#include <string>
#include <vector>
#include <map>
#include <set>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/static_assert.hpp>

// get rid of "unused variable" warnings
#define FB_UNUSED_VARIABLE(x) ((void)(x))

namespace FB
{
    class BrowserHost;
    class JSAPI;
    class JSObject;
    class variant;
    
    // Variant list

    typedef std::vector<variant> VariantList;
    typedef std::map<std::string, variant> VariantMap;
    typedef std::set<std::string> StringSet;
    
    // FB pointer types
    
    typedef boost::shared_ptr<FB::JSAPI> JSAPIPtr; 
    typedef boost::shared_ptr<FB::JSObject> JSObjectPtr;
    typedef boost::shared_ptr<FB::BrowserHost> BrowserHostPtr;
    
    // backwards compability typedefs
    
    typedef BrowserHost BrowserHostWrapper;
    typedef JSObject BrowserObjectAPI;
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
    
    template<class T, class U> 
    boost::shared_ptr<T> ptr_cast(boost::shared_ptr<U> const & r);

    // helper type to allow JSAPIAuto catching of a list of variant arguments
    
    struct CatchAll {
        typedef FB::VariantList value_type;
        FB::VariantList value;
    };

    // JSAPI methods

    class JSAPI;
    typedef variant (JSAPI::*CallMethodPtr)(const std::vector<variant>&);
    struct MethodInfo {
        MethodInfo() : callFunc(NULL) { }
        MethodInfo(CallMethodPtr callFunc) : callFunc(callFunc) { }
        MethodInfo(const MethodInfo &rh) : callFunc(rh.callFunc) { }
        CallMethodPtr callFunc;
    };


    typedef std::map<std::string, MethodInfo> MethodMap;

    // JSAPI properties

    typedef variant (JSAPI::*GetPropPtr)();
    typedef void (JSAPI::*SetPropPtr)(const variant& value);
    struct PropertyInfo {
        PropertyInfo() : getFunc(NULL), setFunc(NULL) { }
        PropertyInfo(GetPropPtr getFunc, SetPropPtr setFunc) : getFunc(getFunc), setFunc(setFunc) { }
        PropertyInfo(const PropertyInfo &rh) : getFunc(rh.getFunc), setFunc(rh.setFunc) { }
        GetPropPtr getFunc;
        SetPropPtr setFunc;
    };

    typedef std::map<std::string, PropertyInfo> PropertyMap;

    // new style JSAPI methods

    typedef boost::function<variant (const std::vector<variant>&)> CallMethodFunctor;
    typedef std::map<std::string, CallMethodFunctor> MethodFunctorMap;

    // new style JSAPI properties

    typedef boost::function<FB::variant ()> GetPropFunctor;
    typedef boost::function<void (const FB::variant&)> SetPropFunctor;
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
        enum { base_is_firebreath_class = 
                       boost::is_base_of<JSAPI, T>::value
                    || boost::is_base_of<BrowserHost, T>::value
        };
        // This should only be used with FireBreath' and derived classes
        BOOST_STATIC_ASSERT(base_is_firebreath_class);
        return boost::dynamic_pointer_cast<T>(r);
    }
}

// This needs to be included after all our classes are defined because it relies on types defined in this file
// TODO: can this be done better?
#include "variant.h"

#endif
