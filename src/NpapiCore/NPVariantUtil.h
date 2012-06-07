/**********************************************************\
 Original Author: Georg Fritzsche

 Created:    Nov 6, 2010
 License:    Dual license model; choose one of two:
 New BSD License
 http://www.opensource.org/licenses/bsd-license.php
 - or -
 GNU Lesser General Public License, version 2.1
 http://www.gnu.org/licenses/lgpl-2.1.html

 Copyright 20100 Georg Fritzsche, Firebreath development team
 \**********************************************************/

#pragma once
#ifndef FB_H_NPVARIANTUTIL
#define FB_H_NPVARIANTUTIL

#include <map>
#include <string>
#include <typeinfo>

#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/integer_traits.hpp>

#include "NpapiTypes.h"
#include "APITypes.h"
#include "NpapiPluginModule.h"
#include "NPJavascriptObject.h"
#include "NPObjectAPI.h"
#include "DOM/Document.h"
#include "DOM/Window.h"
#include "variant_list.h"
#include "NpapiBrowserHost.h"


namespace FB { namespace Npapi
{
    struct type_info_less
    {
        bool operator() (const std::type_info* const lhs, const std::type_info* const rhs) const
        {
            return lhs->before(*rhs) ? true : false;
        }
    };

    typedef NPVariant (*NPVariantBuilder)(const NpapiBrowserHostPtr&, const FB::variant&);
    typedef std::map<std::type_info const*, NPVariantBuilder, type_info_less> NPVariantBuilderMap;

    template<class T>
    NPVariant makeNPVariant(const NpapiBrowserHostPtr& host, const FB::variant& var)
    {
        BOOST_STATIC_ASSERT((false && sizeof(T)==0));
        NPVariant npv;
        npv.type = NPVariantType_Null;
        return npv;
    }

    template<> inline
    NPVariant makeNPVariant<double>(const NpapiBrowserHostPtr& host, const FB::variant& var)
    {
        NPVariant npv;
        npv.type = NPVariantType_Double;
        npv.value.doubleValue = var.convert_cast<double>();
        return npv;
    }

    template<> inline
    NPVariant makeNPVariant<int32_t>(const NpapiBrowserHostPtr& host, const FB::variant& var)
    {
        NPVariant npv;
        npv.type = NPVariantType_Int32;
        npv.value.intValue = var.convert_cast<int32_t>();
        return npv;
    }

    template<> inline
    NPVariant makeNPVariant<bool>(const NpapiBrowserHostPtr& host, const FB::variant& var)
    {
        NPVariant npv;
        npv.type = NPVariantType_Bool;
        npv.value.boolValue = var.convert_cast<bool>();
        return npv;
    }

    template<> inline
    NPVariant makeNPVariant<std::string>(const NpapiBrowserHostPtr& host, const FB::variant& var)
    {
        NPVariant npv;

        std::string str = var.convert_cast<std::string>();
        char *outStr = (char*)host->MemAlloc(str.size() + 1);
        memcpy(outStr, str.c_str(), str.size() + 1);
        npv.type = NPVariantType_String;
        npv.value.stringValue.UTF8Characters = outStr;
        npv.value.stringValue.UTF8Length = str.size();

        return npv;
    }

    template<> inline
    NPVariant makeNPVariant<std::wstring>(const NpapiBrowserHostPtr& host, const FB::variant& var)
    {
        NPVariant npv;

        // This is not a typo; the std::string gets the UTF8 representation
        // and we pass that back to the browser
        std::string str = var.convert_cast<std::string>();
        char *outStr = (char*)host->MemAlloc(str.size() + 1);
        memcpy(outStr, str.c_str(), str.size() + 1);
        npv.type = NPVariantType_String;
        npv.value.stringValue.UTF8Characters = outStr;
        npv.value.stringValue.UTF8Length = str.size();

        return npv;
    }

    template<> inline
    NPVariant makeNPVariant<FB::FBVoid>(const NpapiBrowserHostPtr& host, const FB::variant& var)
    {
        NPVariant npv;
        npv.type = NPVariantType_Void;
        return npv;
    }

    template<> inline
    NPVariant makeNPVariant<FB::FBNull>(const NpapiBrowserHostPtr& host, const FB::variant& var)
    {
        NPVariant npv;
        npv.type = NPVariantType_Null;
        return npv;
    }

    template<> inline
    NPVariant makeNPVariant<FB::VariantList>(const NpapiBrowserHostPtr& host, const FB::variant& var)
    {
        NPVariant npv;

        FB::JSObjectPtr outArr = host->getDOMWindow()->createArray();
        FB::VariantList inArr = var.cast<FB::VariantList>();

        for (FB::VariantList::iterator it = inArr.begin(); it != inArr.end(); ++it) {
            outArr->Invoke("push", FB::variant_list_of(*it));
        }

        if (NPObjectAPIPtr api = FB::ptr_cast<NPObjectAPI>(outArr)) {
            npv.type = NPVariantType_Object;
            npv.value.objectValue = api->getNPObject();
            host->RetainObject(npv.value.objectValue);
        }

        return npv;
    }

    template<> inline
    NPVariant makeNPVariant<FB::VariantMap>(const NpapiBrowserHostPtr& host, const FB::variant& var)
    {
        NPVariant npv;

        FB::JSObjectPtr out = host->getDOMWindow()->createMap();
        FB::VariantMap inMap = var.cast<FB::VariantMap>();

        for (FB::VariantMap::iterator it = inMap.begin(); it != inMap.end(); ++it) {
            out->SetProperty(it->first, it->second);
        }

        if (NPObjectAPIPtr api = FB::ptr_cast<NPObjectAPI>(out)) {
            npv.type = NPVariantType_Object;
            npv.value.objectValue = api->getNPObject();
            host->RetainObject(npv.value.objectValue);
        }

        return npv;
    }

//    template<> inline
//    NPVariant makeNPVariant<FB::FBDateString>(const NpapiBrowserHostPtr& host, const FB::variant& var)
//    {
//        NPVariant npv;
//
//        FB::FBDateString date = var.cast<FB::FBDateString>();
//        FB::JSObjectPtr out = host->getDOMWindow()->createDate(date.getValue());;
//
//        if (NPObjectAPIPtr api = FB::ptr_cast<NPObjectAPI>(out)) {
//            npv.type = NPVariantType_Object;
//            npv.value.objectValue = api->getNPObject();
//            host->RetainObject(npv.value.objectValue);
//        } else {
//            return makeNPVariant<std::string>(host, date.getValue());
//        }
//
//        return npv;
//    }

    template<> inline
    NPVariant makeNPVariant<FB::JSAPIPtr>(const NpapiBrowserHostPtr& host, const FB::variant& var)
    {
        NPVariant npv;

        if (FB::JSAPIPtr obj = var.cast<FB::JSAPIPtr>())
        {
            NPObject *outObj = NULL;

            if (NPObjectAPIPtr tmpObj = FB::ptr_cast<NPObjectAPI>(obj)) {
                outObj = tmpObj->getNPObject();
                host->RetainObject(outObj);
            } else {
                outObj = host->getJSAPIWrapper(var.cast<FB::JSAPIPtr>(), true);
            }
            if (outObj) {
                npv.type = NPVariantType_Object;
                npv.value.objectValue = outObj;
            } else {
                npv.type = NPVariantType_Null;
            }
        }
        else
        {
            npv.type = NPVariantType_Null;
        }

        return npv;
    }
    template<> inline
    NPVariant makeNPVariant<FB::JSAPIWeakPtr>(const NpapiBrowserHostPtr& host, const FB::variant& var)
    {
        NPVariant npv;

        // convert_cast will automatically make a shared_ptr out of the weak_ptr; if the weak_ptr
        // is expired it'll be a NULL ptr
        if (FB::JSAPIPtr obj = var.convert_cast<FB::JSAPIPtr>())
        {
            NPObject *outObj = NULL;

            if (NPObjectAPIPtr tmpObj = FB::ptr_cast<NPObjectAPI>(obj)) {
                outObj = tmpObj->getNPObject();
                host->RetainObject(outObj);
            } else {
                // The big difference between returning a weak_ptr instead of a shared_ptr is
                // that we don't have the browser object retain the weak_ptr
                outObj = host->getJSAPIWrapper(var.cast<FB::JSAPIWeakPtr>());
            }

            npv.type = NPVariantType_Object;
            npv.value.objectValue = outObj;
        }
        else
        {
            npv.type = NPVariantType_Null;
        }

        return npv;
    }

    template<> inline
    NPVariant makeNPVariant<FB::JSObjectPtr>(const NpapiBrowserHostPtr& host, const FB::variant& var)
    {
        NPVariant npv;

        if (FB::JSObjectPtr obj = var.cast<FB::JSObjectPtr>())
        {
            NPObject *outObj = NULL;

            if (NPObjectAPIPtr tmpObj = FB::ptr_cast<NPObjectAPI>(obj)) {
                outObj = tmpObj->getNPObject();
                host->RetainObject(outObj);
            } else {
                outObj = NPJavascriptObject::NewObject(host, obj, true);
            }

            npv.type = NPVariantType_Object;
            npv.value.objectValue = outObj;
        }
        else
        {
            npv.type = NPVariantType_Null;
        }

        return npv;
    }

    namespace select_npvariant_builder
    {
        template<class T>
        NPVariantBuilder isIntegral(const boost::true_type& /* is_integral */)
        {
            if (boost::is_same<T, bool>::value) {
                return &makeNPVariant<bool>;
            }

            if ((boost::is_signed<T>::value && (sizeof(T) <= sizeof(int32_t))) || (boost::is_unsigned<T>::value && (sizeof(T) <= sizeof(int32_t)/2))) {
                // max value of T fits into int32_t
                return &makeNPVariant<int32_t>;
            }

            // max value of T doesn't fit into int32_t, use string instead
            return &makeNPVariant<std::string>;
        }

        template<class T>
        NPVariantBuilder isIntegral(const boost::false_type& /* is_integral */)
        {
            BOOST_STATIC_ASSERT((boost::is_floating_point<T>::value));
            return &makeNPVariant<double>;
        }

        template<class T>
        NPVariantBuilder isArithmetic(const boost::true_type& /* is_arithmetic */)
        {
            return isIntegral<T>(boost::is_integral<T>());
        }

        template<class T>
        NPVariantBuilder isArithmetic(const boost::false_type& /* is_arithmetic */)
        {
            return &makeNPVariant<T>;
        }

        template<class T>
        NPVariantBuilder select()
        {
            return isArithmetic<T>(boost::is_arithmetic<T>());
        }
    }
} }

#endif

