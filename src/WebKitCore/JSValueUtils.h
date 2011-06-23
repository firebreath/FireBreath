/**********************************************************\
 Original Author: Richard Bateman
 
 Created:    Jun 7, 2011
 License:    Dual license model; choose one of two:
             New BSD License
             http://www.opensource.org/licenses/bsd-license.php
             - or -
             GNU Lesser General Public License, version 2.1
             http://www.gnu.org/licenses/lgpl-2.1.html
 
 Copyright 2011 Facebook, Inc and Firebreath development team
\**********************************************************/

#pragma once
#ifndef FB_H_FBWebKitUTIL
#define FB_H_FBWebKitUTIL

#include <map>
#include <string>
#include <typeinfo>

#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/integer_traits.hpp>
#include <boost/scoped_array.hpp>

#include "APITypes.h"
#include "DOM/Document.h"
#include "DOM/Window.h"
#include "variant_list.h"
#include "WebKitBrowserHost.h"
#include "JSObjectRefObject.h"
#include "JSObjectRefAPI.h"

namespace FB { namespace WebKit
    {
        struct type_info_less
        {
            bool operator() (const std::type_info* const lhs, const std::type_info* const rhs) const
            {
                return lhs->before(*rhs) ? true : false;
            }
        };
        
        typedef JSValueRef (*JSValueConverter)(const WebKitBrowserHostPtr&, const FB::variant&);
        typedef std::map<std::type_info const*, JSValueConverter, type_info_less> JSValueConverterMap;
        
        template<class T>
        JSValueRef makeJSValueType(const WebKitBrowserHostPtr& host, const FB::variant& var)
        {
            BOOST_STATIC_ASSERT((false && sizeof(T)==0));
            return JSValueMakeUndefined(host->getJSContext());
        }
        
        template<> inline
        JSValueRef makeJSValueType<double>(const WebKitBrowserHostPtr& host, const FB::variant& var)
        {
            return JSValueMakeNumber(host->getJSContext(), var.convert_cast<double>());
        }
        
        template<> inline
        JSValueRef makeJSValueType<int32_t>(const WebKitBrowserHostPtr& host, const FB::variant& var)
        {
            return JSValueMakeNumber(host->getJSContext(), var.convert_cast<int32_t>());
        }
        
        template<> inline
        JSValueRef makeJSValueType<bool>(const WebKitBrowserHostPtr& host, const FB::variant& var)
        {
            return JSValueMakeBoolean(host->getJSContext(), var.convert_cast<bool>());
        }
        
        template<> inline
        JSValueRef makeJSValueType<std::string>(const WebKitBrowserHostPtr& host, const FB::variant& var)
        {
            JSStringRef str = JSStringCreateWithUTF8CString(var.convert_cast<std::string>().c_str());
            JSValueRef retVal = JSValueMakeString(host->getJSContext(), str);
            JSStringRelease(str);
            return retVal;
        }
        
        template<> inline
        JSValueRef makeJSValueType<std::wstring>(const WebKitBrowserHostPtr& host, const FB::variant& var)
        {
            // This is not a typo; the std::string gets the UTF8 representation
            // and we pass that back to the browser
            return makeJSValueType<std::string>(host, var);
        }
        
        template<> inline
        JSValueRef makeJSValueType<FB::FBVoid>(const WebKitBrowserHostPtr& host, const FB::variant& var)
        {
            return JSValueMakeUndefined(host->getJSContext());
        }
        
        template<> inline
        JSValueRef makeJSValueType<FB::FBNull>(const WebKitBrowserHostPtr& host, const FB::variant& var)
        {
            return JSValueMakeNull(host->getJSContext());
        }
        
        template<> inline
        JSValueRef makeJSValueType<FB::VariantList>(const WebKitBrowserHostPtr& host, const FB::variant& var)
        {
            FB::VariantList l = var.cast<FB::VariantList>();
            size_t count = l.size();
            JSContextRef jsContext = host->getJSContext();
            
            boost::scoped_array<JSValueRef> jsArgs(new JSValueRef[count]);
            for (size_t i = 0; i < count; ++i) {
                jsArgs[i] = host->JSValueRefFromVariant(l[i]);
            }
            return JSObjectMakeArray(jsContext, count, jsArgs.get(), NULL);
        }
        
        template<> inline
        JSValueRef makeJSValueType<FB::VariantMap>(const WebKitBrowserHostPtr& host, const FB::variant& var)
        {
            FB::VariantMap m = var.cast<FB::VariantMap>();
            JSContextRef jsContext = host->getJSContext();
            JSObjectRef outObj = JSObjectMake(jsContext, NULL, NULL);
            FB::VariantMap::iterator it(m.begin());
            FB::VariantMap::iterator end(m.end());
            while (it != end) {
                JSStringRef pName = JSStringCreateWithUTF8CString(it->first.c_str());
                JSObjectSetProperty(jsContext, outObj, pName, host->JSValueRefFromVariant(it->second), 0, NULL);
                JSStringRelease(pName);
                ++it;
            }
            
            return outObj;
        }
        
        template<> inline
        JSValueRef makeJSValueType<FB::JSAPIPtr>(const WebKitBrowserHostPtr& host, const FB::variant& var)
        {
            FB::JSAPIPtr api(var.convert_cast<FB::JSAPIPtr>());
            if (!api) {
                return JSValueMakeNull(host->getJSContext());
            }
            host->retainJSAPIPtr(api);
            return JSObjectRefObject::NewObject(host, api);
        }
        template<> inline
        JSValueRef makeJSValueType<FB::JSAPIWeakPtr>(const WebKitBrowserHostPtr& host, const FB::variant& var)
        {
            FB::JSAPIPtr api(var.convert_cast<FB::JSAPIPtr>());
            if (!api) {
                return JSValueMakeNull(host->getJSContext());
            }
            return JSObjectRefObject::NewObject(host, api);
        }
        
        template<> inline
        JSValueRef makeJSValueType<FB::JSObjectPtr>(const WebKitBrowserHostPtr& host, const FB::variant& var)
        {
            FB::JSObjectPtr oPtr(var.cast<FB::JSObjectPtr>());
            
            JSObjectRefAPIPtr api(FB::ptr_cast<JSObjectRefAPI>(oPtr));
            if (api) {
                JSObjectRef o(api->getJSObjectRef());
                if (o) return o;
                else return JSValueMakeNull(host->getJSContext());
            }
            
            return makeJSValueType<FB::JSAPIPtr>(host, var);
        }
        
        namespace select_JSValueRef_builder
        {
            template<class T>
            JSValueConverter isIntegral(const boost::true_type& /* is_integral */)
            {
                if (boost::is_same<T, bool>::value) {
                    return &makeJSValueType<bool>;
                }
                
                if ((boost::is_signed<T>::value && (sizeof(T) <= sizeof(int32_t))) || (boost::is_unsigned<T>::value && (sizeof(T) <= sizeof(int32_t)/2))) {
                    // max value of T fits into int32_t
                    return &makeJSValueType<int32_t>;
                }
                
                // max value of T doesn't fit into int32_t, use string instead
                return &makeJSValueType<std::string>;
            }
            
            template<class T>
            JSValueConverter isIntegral(const boost::false_type& /* is_integral */)
            {
                BOOST_STATIC_ASSERT((boost::is_floating_point<T>::value));
                return &makeJSValueType<double>;
            }
            
            template<class T>
            JSValueConverter isArithmetic(const boost::true_type& /* is_arithmetic */)
            {
                return isIntegral<T>(boost::is_integral<T>());
            }
            
            template<class T>
            JSValueConverter isArithmetic(const boost::false_type& /* is_arithmetic */)
            {
                return &makeJSValueType<T>;
            }
            
            template<class T>
            JSValueConverter select()
            {
                return isArithmetic<T>(boost::is_arithmetic<T>());
            }
        }
    } }

#endif // FB_H_FBWebKitUTIL

